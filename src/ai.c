/*
 * AI routines
 *
 * Strategy: forward-simulate the game physics for a range of candidate
 * action plans, score each simulated outcome, and execute the best plan's
 * first-tick action.
 *
 * A "plan" is:
 *   - ground_direction: which way to accelerate while on the ground (-1/+1)
 *   - accel_ticks:      how many ticks to hold that direction before jumping
 *   - switch_on_wall:   if true, flip the ground direction on wall contact
 *                       (enables wall-bounce speed building)
 *   - air_direction:    which way to hold in the air (-1, 0, +1)
 *
 * Re-planning discipline:
 *   - Grounded: re-plan every tick.
 *   - Airborne: hold the committed air_direction from takeoff; no re-plan.
 *
 * The simulator mirrors update_gameplay's physics exactly.
 *
 * Debug output can be enabled by compiling with -DAI_DEBUG. This exposes
 * an F2 toggle key that switches stderr logging on and off at runtime.
 */

#define AI_DEBUG

#include <allegro5/allegro.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#ifdef AI_DEBUG
#include <stdio.h>
#include <time.h>
#endif

#include "ai.h"
#include "controls.h"
#include "gameplay.h"
#include "keyboard.h"
#include "level.h"
#include "random.h"
#include "scene.h"

/* --- Physics constants --- */

#define GRAVITY             0.8
#define MAX_DX              12.2
#define MIN_JUMP_MAG        12.2
#define SUPER_JUMP_MAG      22.0
#define X_LEFT_WALL         85.0
#define X_RIGHT_WALL        555.0
#define PLAYER_HALF_WIDTH   11
#define DEATH_Y             540.0

/* --- Key bindings --- */

#define AI_TOGGLE_KEY       ALLEGRO_KEY_F1
#ifdef AI_DEBUG
#define AI_DEBUG_KEY        ALLEGRO_KEY_F2
#endif

/* --- Search space --- */

#define MAX_ACCEL_TICKS     60
#define MAX_FLIGHT_TICKS    200

/* --- Second-jump lookahead --- */

/* Number of first-plan candidates that receive second-jump lookahead. */
#define LOOKAHEAD_TOP_N     20

/* Number of second-plan candidates per first-plan that receive
 * third-jump lookahead. */
#define LOOKAHEAD_TOP_K2    5

/* Number of distinct accel_ticks values sampled in the coarse second-
 * and third-jump searches. Combined with 2 directions x 2 switch x 3
 * air dirs, this gives 2 * COARSE_MAX_ACCEL_STEPS * 2 * 3 = 60 candidate
 * plans per coarse search. */
#define COARSE_MAX_ACCEL_STEPS 5
static const int coarse_accel_ticks[COARSE_MAX_ACCEL_STEPS] = {0, 5, 15, 30, 60};

/* --- AI state --- */

static bool ai_enabled = false;

static bool ai_current_keys[AI_KEY_MAX];
static bool ai_target_keys[AI_KEY_MAX];

static bool jump_released_since_last_jump = true;

static bool have_air_commit = false;
static int  committed_air_direction = 0;
/* On the first airborne tick after takeoff, skip wall-flip detection.
 * The sim's Phase 2 (jump tick) doesn't apply switch_on_wall — its
 * `bounced` return is discarded. So if a wall clamp happens on the
 * takeoff tick, the executor must not flip air_direction based on it,
 * otherwise sim and reality diverge. */
static bool skip_first_airborne_wall_flip = false;

/* Track whether the most recent grounded plan wants wall-switching, and
 * remember our currently held ground direction so we can flip it in
 * response to real-game wall contacts.
 *
 * Wall bounces are edge-triggered on the transition from "not at wall"
 * to "at wall": we flip direction only on the tick the character's x
 * gets clamped to a wall, not on every tick it happens to be near one.
 * Level-triggered detection (e.g. "at wall AND dx flipped") produces
 * spurious flips when dx oscillates around zero while the character
 * sits against a wall — this was the "not moving in air" bug. */
static bool current_switch_on_wall = false;
static int  current_ground_direction = 0;
static bool prev_at_wall = false;

#ifdef AI_DEBUG

static bool ai_debug = false;

/* Per-tick AI update timing. Aggregated over a window and reported
 * periodically to stderr so we can gauge how much time the AI search
 * costs, and estimate feasibility of deeper lookahead. */
static double ai_time_min = 1e9;
static double ai_time_max = 0.0;
static double ai_time_sum = 0.0;
static int    ai_time_count = 0;

static double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

static void sample_ai_time(double t_start) {
    double t_end = get_time_ms();
    double elapsed = t_end - t_start;
    if (elapsed < ai_time_min) ai_time_min = elapsed;
    if (elapsed > ai_time_max) ai_time_max = elapsed;
    ai_time_sum += elapsed;
    ai_time_count++;
    if (ai_time_count >= 60) {
        fprintf(stderr,
            "AI timing: min=%.2fms max=%.2fms avg=%.2fms (over %d ticks)\n",
            ai_time_min, ai_time_max, ai_time_sum / ai_time_count,
            ai_time_count);
        ai_time_min = 1e9;
        ai_time_max = 0.0;
        ai_time_sum = 0.0;
        ai_time_count = 0;
    }
}

/* Tracks the ground phase (from landing to takeoff) for debugging. */
static int  debug_ground_ticks = 0;
static int  debug_ground_flips = 0;
static int  debug_last_ground_dir = 0;
static int  debug_ground_start_combo_timer = 0;
static int  debug_ground_start_combo_current = 0;

/* Tracks the flight (takeoff to landing) for debugging. */
static bool debug_pending_landing = false;
static int  debug_pre_takeoff_level = 0;
static int  debug_predicted_levels = 0;
static double debug_predicted_land_dx = 0.0;
static int  debug_pre_takeoff_combo = 0;

#endif /* AI_DEBUG */

/* --- Simulator state --- */

struct sim_state {
    double x, y, dx, dy;
    enum jump_state jump_state;
    int screen_y;
    int prev_x, prev_y, prev_screen_y;
    bool jumped;
    int ticks;
    int clock_ticks;
    int clock_speed;
    int last_level;
    int combo_timer;
    int combo_current;
    int jump_streak;
    struct platform platforms[32];
    unsigned int rng_state;
};

static int sim_rand_msvc(unsigned int *state) {
    *state = *state * 214013u + 2531011u;
    return (*state >> 16) & 0x7fff;
}

/*
 * Local mirror of generate_new_platform() from level.c, operating on the
 * sim's private platform array and RNG state. Must stay in sync with
 * level.c — any change to platform generation there must be reflected here.
 */
static void sim_generate_new_platform(struct sim_state *s) {
    memmove(s->platforms, s->platforms + 1,
            sizeof(s->platforms) - sizeof(struct platform));
    if (s->platforms[31].pad == 4) {
        int level = ++s->platforms[31].level;
        s->platforms[31].pad = 0;
        if (level <= 1000 ? (level % 50 == 0) : (level % 500 == 0)) {
            s->platforms[31].start = 0;
            s->platforms[31].end = 40;
        } else {
            int start;
            int length;
            if (level < 600) {
                long double divisor = (long double)(300 - level) * 0.03333333333333333;
                length = 6 + sim_rand_msvc(&s->rng_state)
                             % (int)(divisor >= 1.0 ? divisor : 1.0);
            } else if (level <= 1000) {
                length = 6;
            } else if (level <= 1500) {
                length = 5;
            } else if (level <= 2000) {
                length = 4;
            } else if (level <= 10000) {
                length = 3;
            } else {
                length = 2;
            }
            start = 5 + sim_rand_msvc(&s->rng_state) % (30 - length);
            s->platforms[31].start = start;
            s->platforms[31].end = start + length;
        }
    } else {
        s->platforms[31].pad++;
    }
}

static bool sim_get_platform(const struct sim_state *s, int y,
                             int *platform_y, int *platform_x_left,
                             int *platform_x_right, int *platform_level)
{
    unsigned int i;
    const struct platform *p;
    if (y < -33 || y > 478)
        return false;
    i = 31 - (y + 33) / 16;
    p = &s->platforms[i];
    if (p->pad == 0) {
        *platform_y = s->screen_y % 16 + 16 * (29 - (int)i);
        *platform_x_left = 16 * p->start - 2;
        *platform_x_right = 16 * p->end + 17;
        *platform_level = p->level;
        return true;
    }
    return false;
}

static int sim_get_level(const struct sim_state *s, int y) {
    unsigned int i;
    if (y < -33 || y > 478)
        return 0;
    i = 31 - (y + 33) / 16;
    return s->platforms[i].level;
}

/*
 * Local copy of line_intersection() from gameplay.c. Must stay in sync
 * with gameplay.c — any fix or change to the collision math there must
 * be mirrored here.
 */
static bool line_intersection(int x1, int y1, int x2, int y2,
                              int x3, int y3, int x4, int y4,
                              int *x, int *y)
{
    int denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
    int num_a = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
    int num_b = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);
    double inv_denom;
    double u_a, u_b;
    if (denom == 0)
        return false;
    inv_denom = 1.0 / denom;
    u_a = num_a * inv_denom;
    u_b = num_b * inv_denom;
    if (u_a < 0.0 || u_a > 1.0 || u_b < 0.0 || u_b > 1.0)
        return false;
    *x = x1 + (int)(u_a * (x2 - x1) + 0.5);
    *y = y1 + (int)(u_a * (y2 - y1) + 0.5);
    return true;
}

/*
 * One simulation step. Returns true if a wall bounce occurred this tick
 * (character x was clamped to a wall AND dx reversed sign). Callers can
 * use this to implement plans that switch direction on wall contact.
 */
static bool sim_step(struct sim_state *s, bool left, bool right, bool jump,
                     bool rejump)
{
    bool hit_wall = false;

    /* update_ticks */
    s->ticks++;

    /* update_movement */
    if (left) {
        if (s->dx > 0.0) s->dx *= 0.7;
        s->dx -= 0.3;
    } else if (right) {
        if (s->dx < 0.0) s->dx *= 0.7;
        s->dx += 0.3;
    } else {
        s->dx *= 0.9;
    }
    if (jump && (rejump || !s->jumped) && s->jump_state == JUMP_STATE_IDLE) {
        s->jumped = true;
        s->jump_state = JUMP_STATE_FLY_UP;
        s->dy = (s->dx < 0.0) ? (2.0 * s->dx) : (-2.0 * s->dx);
        if (s->dy > -MIN_JUMP_MAG) s->dy = -MIN_JUMP_MAG;
    } else if (!jump) {
        s->jumped = false;
    }

    /* update_position */
    s->prev_x = (int)s->x;
    s->prev_y = (int)s->y;
    if (s->dx > MAX_DX) s->dx = MAX_DX;
    else if (s->dx < -MAX_DX) s->dx = -MAX_DX;
    if (s->dy > MAX_DX) s->dy = MAX_DX;
    else if (s->dy < -100.0) s->dy = -100.0;
    s->x += s->dx;
    s->y += s->dy;
    if (s->x > X_RIGHT_WALL) {
        s->x = X_RIGHT_WALL;
        s->dx = s->dx * -0.9;
        hit_wall = true;
    } else if (s->x < X_LEFT_WALL) {
        s->x = X_LEFT_WALL;
        s->dx = s->dx * -0.9;
        hit_wall = true;
    }
    if (s->jump_state != JUMP_STATE_IDLE) {
        s->dy += GRAVITY;
        if (s->jump_state == JUMP_STATE_FLY_UP && s->dy > 0.0)
            s->jump_state = JUMP_STATE_FLY_DOWN;
    }

    /* update_screen */
    {
        int screen_dy;
        s->prev_screen_y = s->screen_y;
        if (s->y < 0.0)        screen_dy = 13;
        else if (s->y < 20.0)  screen_dy = 10;
        else if (s->y < 40.0)  screen_dy = 8;
        else if (s->y < 60.0)  screen_dy = 6;
        else if (s->y < 80.0)  screen_dy = 5;
        else if (s->y < 100.0) screen_dy = 4;
        else if (s->y < 120.0) screen_dy = 3;
        else if (s->y < 140.0) screen_dy = 2;
        else if (s->y < 160.0) screen_dy = 1;
        else                   screen_dy = 0;
        s->screen_y += screen_dy;
        s->y += screen_dy;
        s->prev_y += screen_dy;
    }

    /* update_clock */
    if (s->screen_y > 100) {
        int dy;
        s->clock_ticks++;
        if (s->clock_speed == 0)
            dy = s->ticks % 2;
        else
            dy = s->clock_speed;
        s->screen_y += dy;
        s->y += dy;
        s->prev_y += dy;
    }
    if (s->clock_speed < 5) {
        if (s->clock_ticks > 1500 * (s->clock_speed + 1))
            s->clock_speed++;
    } else {
        s->clock_ticks -= 45;
    }

    /* update_platforms */
    if ((s->screen_y % 16 < s->prev_screen_y % 16) ||
        (s->screen_y - s->prev_screen_y >= 16))
        sim_generate_new_platform(s);

    /* update_collisions */
    {
        int x = (int)s->x;
        int y = (int)s->y;
        int platform_y;
        int platform_x_left, platform_x_right;
        int platform_level;
        bool collision_left, collision_right;
        int cxl, cyl, cxr, cyr;
        bool have_platform =
            sim_get_platform(s, y, &platform_y, &platform_x_left,
                             &platform_x_right, &platform_level) ||
            sim_get_platform(s, s->prev_y, &platform_y, &platform_x_left,
                             &platform_x_right, &platform_level);
        if (!have_platform) {
            if (s->jump_state == JUMP_STATE_IDLE ||
                s->jump_state == JUMP_STATE_FLY_DOWN)
                s->jump_state = JUMP_STATE_FLY_DOWN_2;
        } else {
            collision_left = line_intersection(
                platform_x_left, platform_y, platform_x_right, platform_y,
                x - PLAYER_HALF_WIDTH, y + 1,
                s->prev_x - PLAYER_HALF_WIDTH, s->prev_y, &cxl, &cyl);
            collision_right = line_intersection(
                platform_x_left, platform_y, platform_x_right, platform_y,
                x + PLAYER_HALF_WIDTH, y + 1,
                s->prev_x + PLAYER_HALF_WIDTH, s->prev_y, &cxr, &cyr);
            if (collision_left || collision_right) {
                if (s->jump_state == JUMP_STATE_FLY_DOWN ||
                    s->jump_state == JUMP_STATE_FLY_DOWN_2)
                {
                    s->jump_state = JUMP_STATE_IDLE;
                    s->y = platform_y - 1;
                    s->dy = 0.0;
                    if (collision_left)
                        s->x = cxl + PLAYER_HALF_WIDTH;
                    else
                        s->x = cxr - PLAYER_HALF_WIDTH;
                }
            } else {
                if (s->jump_state == JUMP_STATE_IDLE ||
                    s->jump_state == JUMP_STATE_FLY_DOWN)
                    s->jump_state = JUMP_STATE_FLY_DOWN_2;
            }
        }
    }

    /* update_score (partial: combo bookkeeping only) */
    if (s->combo_timer != 0) {
        s->combo_timer--;
    }
    if (s->jump_state == JUMP_STATE_IDLE) {
        int level = sim_get_level(s, (int)s->y);
        if (level >= s->last_level + 2) {
            if (s->combo_timer != 0) {
                s->combo_current += level - s->last_level;
                s->jump_streak++;
            } else {
                s->combo_current = level - s->last_level;
                s->jump_streak = 1;
            }
            s->combo_timer = 100;
        } else if (level != s->last_level) {
            if (s->combo_timer != 0)
                s->combo_timer = 1;
        }
        s->last_level = level;
    }

    return hit_wall;
}

static void snapshot_state(struct sim_state *s) {
    s->x = g_x;
    s->y = g_y;
    s->dx = g_dx;
    s->dy = g_dy;
    s->jump_state = g_jump_state;
    s->screen_y = g_screen_y;
    s->prev_x = (int)g_x;
    s->prev_y = (int)g_y;
    s->prev_screen_y = g_screen_y;
    s->jumped = !jump_released_since_last_jump;
    s->ticks = 0;
    s->clock_ticks = g_clock_ticks;
    s->clock_speed = g_clock_speed;
    s->last_level = g_last_level;
    s->combo_timer = g_combo_timer;
    s->combo_current = g_combo_current;
    s->jump_streak = 0;
    memcpy(s->platforms, g_platforms, sizeof(s->platforms));
    s->rng_state = get_rand_msvc_state();
}

/* --- Plans and outcomes --- */

struct plan {
    int ground_direction;
    int accel_ticks;
    bool switch_on_wall;                /* wall-bounce: flip dir on wall hit */
    int air_direction;
};

struct outcome {
    bool died;
    bool landed;
    bool fell_off_edge;
    int start_level;
    int end_level;
    double landing_dx;
    int total_ticks;
    int combo_at_end;
    int combo_timer_at_end;
};

static void simulate_plan(const struct sim_state *s0, const struct plan *plan,
                          struct outcome *o, bool rejump,
                          struct sim_state *end_state)
{
    struct sim_state s = *s0;
    int t;
    int dir = plan->ground_direction;

    o->died = false;
    o->landed = false;
    o->fell_off_edge = false;
    o->start_level = s.last_level;
    o->end_level = s.last_level;
    o->landing_dx = 0.0;
    o->total_ticks = 0;
    o->combo_at_end = s.combo_current;

    /* Phase 1: acceleration on the ground. */
    if (s.jump_state == JUMP_STATE_IDLE) {
        for (t = 0; t < plan->accel_ticks; ++t) {
            bool L = dir < 0;
            bool R = dir > 0;
            bool bounced = sim_step(&s, L, R, false, rejump);
            o->total_ticks++;
            if (s.y > DEATH_Y) { o->died = true; goto done; }
            if (s.jump_state != JUMP_STATE_IDLE) {
                o->fell_off_edge = true;
                break;
            }
            if (bounced && plan->switch_on_wall)
                dir = -dir;
        }
    }

    /* Phase 2: jump tick. */
    if (!o->fell_off_edge) {
        bool L = dir < 0;
        bool R = dir > 0;
        sim_step(&s, L, R, true, rejump);
        o->total_ticks++;
        if (s.y > DEATH_Y) { o->died = true; goto done; }
    }

    /* Phase 3: flight. */
    {
        int air_dir = plan->air_direction;
        for (t = 0; t < MAX_FLIGHT_TICKS; ++t) {
            bool L = air_dir < 0;
            bool R = air_dir > 0;
            bool bounced = sim_step(&s, L, R, false, rejump);
            o->total_ticks++;
            if (s.y > DEATH_Y) { o->died = true; goto done; }
            if (s.jump_state == JUMP_STATE_IDLE) {
                o->landed = true;
                o->landing_dx = s.dx;
                o->end_level = s.last_level;
                o->combo_at_end = s.combo_current;
                o->combo_timer_at_end = s.combo_timer;
                goto done;
            }
            if (bounced && plan->switch_on_wall && air_dir != 0)
                air_dir = -air_dir;
        }
    }

done:
    if (end_state)
        *end_state = s;
    return;
}

/*
 * Score an outcome. Higher is better.
 *
 * Priorities (in order of magnitude):
 *   1. Avoid death (dominant negative).
 *   2. Avoid walking off the edge without jumping.
 *   3. Gain many levels per jump — this is the primary reward.
 *   4. Explicit penalty for jumps that don't gain levels (no in-place hops).
 *   5. Combo bonus for multi-level jumps (quadratic in levels gained).
 *   6. Landing momentum: preserving |dx| makes the next jump easier.
 *   7. Time penalty: faster climbing is better, and long roundabout plans
 *      (wall bounces, big reversals) should only win when clearly worth it.
 *   8. Reversal penalty: reversing existing horizontal momentum wastes
 *      speed (0.7x damping), so plans that fight the current dx direction
 *      are penalized proportionally to how much momentum they waste.
 *   9. Combo preservation: if a combo of size N is active at plan start,
 *      penalize plans that don't cleanly extend it. A combo is extended
 *      only when the combo counter grows AND the timer at landing is 100
 *      (fresh). Otherwise the combo is dying — skip-1 landings set the
 *      timer to 1 (dies next tick), long plans let the timer expire
 *      mid-flight so the landing resets combo_current, and skip-0 lets
 *      the timer decay naturally. Penalty is N² * 20 capped at 30000,
 *      scaling aggressively so big combos (25+) are strongly protected.
 *      Composite scoring (see search_best_plan) relies on this check
 *      being strict: without the timer==100 requirement, the second-jump
 *      lookahead would falsely see skip-1 plans as combo-safe, leading
 *      the outer AI to pick first-plan traps whose apparent composite
 *      is inflated by miscounted skip-1 follow-ups.
 */
static double score_outcome(const struct outcome *o,
                            double initial_dx, int ground_dir,
                            int combo_size_at_start)
{
    double score = 0.0;
    int levels_gained;
    if (o->died) return -1e9;
    if (!o->landed) return -1e6;
    if (o->fell_off_edge) score -= 10000.0;
    levels_gained = o->end_level - o->start_level;
    score += levels_gained * 1000.0;
    if (levels_gained <= 0)
        score -= 5000.0;
    if (levels_gained >= 2)
        score += levels_gained * levels_gained * 200.0;
    score += fabs(o->landing_dx) * 5.0;
    score -= o->total_ticks * 3.0;
    if (initial_dx * ground_dir < 0)
        score -= fabs(initial_dx) * 50.0;
    if (combo_size_at_start >= 2) {
        bool combo_extended = (o->combo_at_end > combo_size_at_start) &&
                              (o->combo_timer_at_end == 100);
        if (!combo_extended) {
            double penalty = (double)combo_size_at_start *
                             combo_size_at_start * 20.0;
            if (penalty > 30000.0) penalty = 30000.0;
            score -= penalty;
        }
    }
    return score;
}

#ifdef AI_DEBUG
/*
 * Diagnostic information about a single search invocation. Populated by
 * search_best_plan when a non-NULL pointer is passed, and consumed by
 * debug_print_takeoff to log what the AI decided and considered.
 */
struct plan_debug {
    struct plan best_plan;
    struct outcome best_outcome;
    double best_score;
    struct plan best_skip2_plan;
    struct outcome best_skip2_outcome;
    double best_skip2_score;
    bool best_skip2_found;
    /* Diagnostic counters over the search space. */
    int count_died;
    int count_fell_off;
    int count_no_land;
    int count_skip_nonpositive;
    int count_skip1;
    int count_skip2plus;
};
#endif /* AI_DEBUG */

/* --- Coarse search --- */

/*
 * Entry recording a candidate second plan for third-jump lookahead. */
struct second_candidate {
    struct outcome o;
    struct sim_state end_state;
    double first_score;    /* score of this second plan itself */
    bool valid;
};

/*
 * Coarse search from a state, computing the best plan score directly OR
 * (if top_out is non-NULL) collecting the top LOOKAHEAD_TOP_K2 landing
 * plans for further recursion. Uses only 60 coarse plans (vs. 732 in
 * the primary search).
 *
 * If top_out == NULL, returns best score only.
 * If top_out != NULL, fills top_out[] with up to LOOKAHEAD_TOP_K2 best
 * landing plans (sorted by score); returns best score across all plans
 * (including non-landing ones).
 */
static double search_coarse(const struct sim_state *s0, bool rejump,
                            struct second_candidate *top_out)
{
    struct plan candidate;
    struct outcome outcome;
    struct sim_state end_state;
    double best_score = -DBL_MAX;
    int dir, ati, ad, sw;
    int combo_size = (s0->combo_timer > 0) ? s0->combo_current : 0;
    int top_count = 0;
    int i;

    if (top_out) {
        for (i = 0; i < LOOKAHEAD_TOP_K2; ++i)
            top_out[i].valid = false;
    }

    for (dir = -1; dir <= 1; dir += 2) {
        for (ati = 0; ati < COARSE_MAX_ACCEL_STEPS; ++ati) {
            for (sw = 0; sw <= 1; ++sw) {
                for (ad = -1; ad <= 1; ++ad) {
                    double score;
                    candidate.ground_direction = dir;
                    candidate.accel_ticks = coarse_accel_ticks[ati];
                    candidate.switch_on_wall = (sw != 0);
                    candidate.air_direction = ad;
                    simulate_plan(s0, &candidate, &outcome, rejump,
                                  top_out ? &end_state : NULL);
                    score = score_outcome(&outcome, s0->dx,
                                          candidate.ground_direction,
                                          combo_size);
                    if (score > best_score)
                        best_score = score;

                    /* Track top-K landing plans for third-jump lookahead. */
                    if (!top_out) continue;
                    if (!outcome.landed || outcome.died || outcome.fell_off_edge)
                        continue;

                    if (top_count < LOOKAHEAD_TOP_K2) {
                        top_out[top_count].o = outcome;
                        top_out[top_count].end_state = end_state;
                        top_out[top_count].first_score = score;
                        top_out[top_count].valid = true;
                        top_count++;
                    } else {
                        /* Replace weakest entry if this one is stronger. */
                        int min_idx = 0;
                        for (i = 1; i < LOOKAHEAD_TOP_K2; ++i) {
                            if (top_out[i].first_score < top_out[min_idx].first_score)
                                min_idx = i;
                        }
                        if (score > top_out[min_idx].first_score) {
                            top_out[min_idx].o = outcome;
                            top_out[min_idx].end_state = end_state;
                            top_out[min_idx].first_score = score;
                            top_out[min_idx].valid = true;
                        }
                    }
                }
            }
        }
    }

    return best_score;
}

/* Entry recording a candidate first plan for the lookahead pass. */
struct lookahead_candidate {
    struct plan p;
    struct outcome o;
    double first_score;
    struct sim_state end_state;
};

/*
 * Search for the best plan. Three-pass strategy:
 *
 * Pass 1: exhaustively score all first-plan candidates with score_outcome
 *   (732 candidates). Track the overall best (fallback) and the top-N
 *   landing candidates for lookahead (combo-preserving when a combo is
 *   active; otherwise any landing).
 *
 * Pass 2 (always, if any landing plans exist): for each top-N first-plan,
 *   run a coarse second search from its ending state, collecting the
 *   top-K2 landing second plans.
 *
 * Pass 3: for each top-K2 second plan, run a coarse third search and
 *   take its best score. The composite value of a first plan is:
 *
 *     composite = first_score +
 *                 max_over_top_K2_seconds (second_score + best_third_score)
 *
 * The AI picks the first plan with the best composite value. This
 * catches multi-jump trap chains where a good first jump lands fine,
 * its follow-up jump also lands fine, but the third jump is doomed.
 * Single-plan or 2-jump lookahead misses these.
 */
static struct plan search_best_plan(const struct sim_state *s0, bool rejump
#ifdef AI_DEBUG
                                    , struct plan_debug *dbg
#endif
                                    )
{
    struct plan best;
    struct plan candidate;
    struct outcome outcome;
    struct sim_state end_state;
    double best_score = -DBL_MAX;
    int dir, at, ad, sw;
    bool airborne = (s0->jump_state != JUMP_STATE_IDLE);
    int combo_size = (s0->combo_timer > 0) ? s0->combo_current : 0;
    bool combo_active = (combo_size >= 2);

    /* Top-N tracking for lookahead. Kept as a min-heap-like array
     * (unsorted; we scan on each insertion). */
    struct lookahead_candidate top[LOOKAHEAD_TOP_N];
    int top_count = 0;
    int i;

#ifdef AI_DEBUG
    double best_skip2_score = -DBL_MAX;
#endif

    best.ground_direction = +1;
    best.accel_ticks = 0;
    best.switch_on_wall = false;
    best.air_direction = 0;

#ifdef AI_DEBUG
    if (dbg) {
        dbg->best_skip2_found = false;
        dbg->best_score = -DBL_MAX;
        dbg->best_skip2_score = -DBL_MAX;
        dbg->count_died = 0;
        dbg->count_fell_off = 0;
        dbg->count_no_land = 0;
        dbg->count_skip_nonpositive = 0;
        dbg->count_skip1 = 0;
        dbg->count_skip2plus = 0;
    }
#endif

    /* Pass 1: exhaustive first-plan search. */
    for (dir = -1; dir <= 1; dir += 2) {
        int accel_max = airborne ? 0 : MAX_ACCEL_TICKS;
        for (at = 0; at <= accel_max; ++at) {
            for (sw = 0; sw <= (airborne ? 0 : 1); ++sw) {
                for (ad = -1; ad <= 1; ++ad) {
                    double score;
                    int levels_gained;
                    bool combo_preserved;

                    candidate.ground_direction = dir;
                    candidate.accel_ticks = at;
                    candidate.switch_on_wall = (sw != 0);
                    candidate.air_direction = ad;

                    /* If we're going to consider this for lookahead we
                     * need the end state; otherwise skip the extra copy. */
                    simulate_plan(s0, &candidate, &outcome, rejump,
                                  combo_active ? &end_state : NULL);
                    score = score_outcome(&outcome, s0->dx,
                                          candidate.ground_direction,
                                          combo_size);

                    if (score > best_score) {
                        best_score = score;
                        best = candidate;
#ifdef AI_DEBUG
                        if (dbg) {
                            dbg->best_plan = candidate;
                            dbg->best_outcome = outcome;
                            dbg->best_score = score;
                        }
#endif
                    }

                    levels_gained = outcome.end_level - outcome.start_level;

#ifdef AI_DEBUG
                    if (outcome.landed && !outcome.died &&
                        levels_gained >= 2 && score > best_skip2_score)
                    {
                        best_skip2_score = score;
                        if (dbg) {
                            dbg->best_skip2_plan = candidate;
                            dbg->best_skip2_outcome = outcome;
                            dbg->best_skip2_score = score;
                            dbg->best_skip2_found = true;
                        }
                    }

                    if (dbg) {
                        int lg = levels_gained;
                        if (outcome.died) dbg->count_died++;
                        else if (!outcome.landed) dbg->count_no_land++;
                        else if (outcome.fell_off_edge) dbg->count_fell_off++;
                        else if (lg <= 0) dbg->count_skip_nonpositive++;
                        else if (lg == 1) dbg->count_skip1++;
                        else dbg->count_skip2plus++;
                    }
#endif

                    /* Consider adding to lookahead top-N. Include any
                     * landing plan (not just combo-preserving ones): when
                     * no combo is active, we still want composite scoring
                     * to avoid landing in doomed follow-up states. */
                    if (!outcome.landed || outcome.died || outcome.fell_off_edge)
                        continue;
                    if (combo_active) {
                        combo_preserved =
                            (outcome.combo_at_end >= combo_size);
                        if (!combo_preserved) continue;
                    }

                    if (top_count < LOOKAHEAD_TOP_N) {
                        top[top_count].p = candidate;
                        top[top_count].o = outcome;
                        top[top_count].first_score = score;
                        top[top_count].end_state = end_state;
                        top_count++;
                    } else {
                        /* Find the current minimum in top[] and replace
                         * if this one is better. */
                        int min_idx = 0;
                        for (i = 1; i < LOOKAHEAD_TOP_N; ++i) {
                            if (top[i].first_score < top[min_idx].first_score)
                                min_idx = i;
                        }
                        if (score > top[min_idx].first_score) {
                            top[min_idx].p = candidate;
                            top[min_idx].o = outcome;
                            top[min_idx].first_score = score;
                            top[min_idx].end_state = end_state;
                        }
                    }
                }
            }
        }
    }

    /* Pass 2+3: two-level lookahead for the top N first-plan landing
     * candidates.
     *
     * For each first plan, we run a coarse second search. For each of
     * the top K2 landing second plans (per first plan), we run a coarse
     * third search and take its best score. The composite value for
     * a first plan is:
     *
     *   composite = first_score + max over top-K2 seconds of
     *                             (second_score + best_third_score)
     *
     * This catches multi-jump trap chains where a good first jump lands
     * fine, and its follow-up jump also lands fine, but the jump after
     * that is doomed. Single-plan or 2-jump lookahead misses these. */
    if (top_count > 0) {
        double best_composite = -DBL_MAX;
        int best_composite_idx = -1;
        struct second_candidate seconds[LOOKAHEAD_TOP_K2];

        for (i = 0; i < top_count; ++i) {
            /* Run coarse second search, collecting top K2. */
            search_coarse(&top[i].end_state, rejump, seconds);

            /* For each landing second candidate, run coarse third
             * search and compute (second + best_third). Take max. */
            double best_second_value = -DBL_MAX;
            int j;
            for (j = 0; j < LOOKAHEAD_TOP_K2; ++j) {
                if (!seconds[j].valid) continue;
                double third_best = search_coarse(&seconds[j].end_state,
                                                  rejump, NULL);
                double second_value = seconds[j].first_score + third_best;
                if (second_value > best_second_value)
                    best_second_value = second_value;
            }

            /* If no second plan landed, use a sentinel very-negative
             * value to reflect that this first plan has no viable
             * follow-up. */
            if (best_second_value == -DBL_MAX)
                best_second_value = -1e9;

            double composite = top[i].first_score + best_second_value;
            if (composite > best_composite) {
                best_composite = composite;
                best_composite_idx = i;
            }
        }

        if (best_composite_idx >= 0) {
            best = top[best_composite_idx].p;
#ifdef AI_DEBUG
            if (dbg) {
                dbg->best_plan = top[best_composite_idx].p;
                dbg->best_outcome = top[best_composite_idx].o;
                dbg->best_score = best_composite;
            }
#endif
        }
    }

    return best;
}

/* --- Key plumbing --- */

static int ai_key_to_keycode(enum ai_key key) {
    switch (key) {
    case AI_KEY_LEFT:  return g_left_key;
    case AI_KEY_RIGHT: return g_right_key;
    case AI_KEY_JUMP:  return g_jump_key;
    default:           return 0;
    }
}

void set_ai_key(enum ai_key key, bool down) {
    ai_target_keys[key] = down;
}

void do_ai_tick() {
    int key;
    for (key = 0; key < AI_KEY_MAX; ++key) {
        if (ai_current_keys[key] != ai_target_keys[key]) {
            ALLEGRO_EVENT_TYPE type = ai_target_keys[key]
                ? ALLEGRO_EVENT_KEY_DOWN
                : ALLEGRO_EVENT_KEY_UP;
            int keycode = ai_key_to_keycode(key);
            ALLEGRO_KEYBOARD_EVENT event = {
                .keycode = keycode,
                .type = type,
            };
            handle_keyboard_event(&event);
            ai_current_keys[key] = ai_target_keys[key];
        }
    }
}

void clear_ai_state() {
    memset(ai_target_keys, 0, sizeof(ai_target_keys));
    do_ai_tick();
    jump_released_since_last_jump = true;
    have_air_commit = false;
    committed_air_direction = 0;
    current_switch_on_wall = false;
    current_ground_direction = 0;
    prev_at_wall = false;
    skip_first_airborne_wall_flip = false;
#ifdef AI_DEBUG
    debug_pending_landing = false;
    debug_ground_ticks = 0;
    debug_ground_flips = 0;
    debug_last_ground_dir = 0;
#endif
}

#ifdef AI_DEBUG
/*
 * Dump the AI's current view of the platform layout. Useful for verifying
 * that the sim's platform positions match what's visible on screen.
 */
static void debug_print_platforms(void) {
    int i;
    fprintf(stderr, "  platforms (idx: y_top x=[left..right] level):\n");
    for (i = 0; i < 32; i++) {
        int py, pxl, pxr;
        if (g_platforms[i].pad != 0) continue;
        py = g_screen_y % 16 + 16 * (29 - i);
        pxl = 16 * g_platforms[i].start - 2;
        pxr = 16 * g_platforms[i].end + 17;
        fprintf(stderr, "    [%2d]: y=%3d x=[%3d..%3d] level=%d\n",
                i, py, pxl, pxr, g_platforms[i].level);
    }
}

static void debug_print_takeoff(const struct sim_state *s0,
                                const struct plan_debug *dbg)
{
    int levels_predicted = dbg->best_outcome.end_level -
                           dbg->best_outcome.start_level;
    int combo_decay = debug_ground_start_combo_timer - s0->combo_timer;
    fprintf(stderr,
        "\nAI takeoff:\n"
        "  ground phase: %d ticks, %d dir-flips, combo_timer %d->%d (decay %d)\n"
        "  state: pos=(%.1f,%.1f) dx=%.2f combo_current=%d combo_timer=%d\n"
        "  chosen plan: dir=%+d accel=%d switch=%d air=%+d\n"
        "  predicted:   levels=%d ticks=%d land_dx=%.2f score=%.1f%s\n",
        debug_ground_ticks, debug_ground_flips,
        debug_ground_start_combo_timer, s0->combo_timer, combo_decay,
        s0->x, s0->y, s0->dx, s0->combo_current, s0->combo_timer,
        dbg->best_plan.ground_direction, dbg->best_plan.accel_ticks,
        dbg->best_plan.switch_on_wall, dbg->best_plan.air_direction,
        levels_predicted,
        dbg->best_outcome.total_ticks, dbg->best_outcome.landing_dx,
        dbg->best_score,
        (s0->combo_timer > 0 && s0->combo_current >= 2) ? " (composite)" : "");
    if (dbg->best_skip2_found) {
        bool same_plan =
            dbg->best_skip2_plan.ground_direction == dbg->best_plan.ground_direction &&
            dbg->best_skip2_plan.accel_ticks == dbg->best_plan.accel_ticks &&
            dbg->best_skip2_plan.switch_on_wall == dbg->best_plan.switch_on_wall &&
            dbg->best_skip2_plan.air_direction == dbg->best_plan.air_direction;
        if (!same_plan) {
            int skip2_levels = dbg->best_skip2_outcome.end_level -
                               dbg->best_skip2_outcome.start_level;
            fprintf(stderr,
                "  best skip-2+ alternative: dir=%+d accel=%d switch=%d air=%+d\n"
                "    predicted: levels=%d ticks=%d score=%.1f (chosen beats by %.1f)\n",
                dbg->best_skip2_plan.ground_direction,
                dbg->best_skip2_plan.accel_ticks,
                dbg->best_skip2_plan.switch_on_wall,
                dbg->best_skip2_plan.air_direction,
                skip2_levels, dbg->best_skip2_outcome.total_ticks,
                dbg->best_skip2_score,
                dbg->best_score - dbg->best_skip2_score);
        }
    } else {
        fprintf(stderr, "  no skip-2+ plan found in search\n");
    }
    fprintf(stderr,
        "  search stats: died=%d fell_off=%d no_land=%d skip<=0=%d skip1=%d skip2+=%d\n",
        dbg->count_died, dbg->count_fell_off, dbg->count_no_land,
        dbg->count_skip_nonpositive, dbg->count_skip1, dbg->count_skip2plus);
    debug_print_platforms();
}
#endif /* AI_DEBUG */

/*
 * Top-level AI tick.
 *
 * When grounded we re-search over plans and execute the first tick.
 * The plan tracks whether to flip direction on wall contact — this is how
 * the AI performs wall-bounce speed building. To execute this correctly
 * outside the simulator, we detect wall contact each real tick and, if
 * the current plan has switch_on_wall set, flip the direction we hold.
 */
void update_ai() {
    struct sim_state s0;
    struct plan plan;
#ifdef AI_DEBUG
    struct plan_debug dbg;
    struct plan_debug *dbg_ptr;
#endif

    if (is_key_pressed(AI_TOGGLE_KEY)) {
        ai_enabled = !ai_enabled;
        if (!ai_enabled)
            clear_ai_state();
    }
#ifdef AI_DEBUG
    if (is_key_pressed(AI_DEBUG_KEY)) {
        ai_debug = !ai_debug;
        fprintf(stderr, "AI debug: %s\n", ai_debug ? "ON" : "OFF");
    }
#endif

    if (!ai_enabled) return;
    if (g_scene != GAMEPLAY_SCENE) return;
    if (g_pause || g_death) return;

#ifdef AI_DEBUG
    double t_start = get_time_ms();
#endif

    /* Airborne: hold the committed air direction, but flip it on wall
     * bounces if the current plan requested wall-switching. Detection is
     * edge-triggered on the not-at-wall -> at-wall transition, matching
     * the sim's "hit_wall this tick" semantics. */
    if (g_jump_state != JUMP_STATE_IDLE && have_air_commit) {
        /* at_wall detects a POST-clamp state: character is exactly at a
         * wall AND dx has the sign we'd expect from a bounce (leftward
         * at right wall, rightward at left wall). This distinguishes
         * "just bounced off wall" from "arrived at exact wall x but not
         * yet clamped" — the physics clamp requires strict inequality
         * (x > 555 or x < 85), so the character can transiently reach
         * exactly 555 or 85 without a clamp happening. Using >= or <=
         * without a dx-sign check would fire spuriously in that case.
         *
         * wall_impact is additionally suppressed on the first airborne
         * tick after takeoff, since the sim's Phase 2 (jump tick) does
         * not apply switch_on_wall — its `bounced` return is discarded.
         * Without this suppression, a wall clamp on the takeoff tick
         * would cause a spurious air-direction flip that the sim didn't
         * predict, diverging trajectory from prediction. */
        bool at_right_wall_bounce = (g_x >= X_RIGHT_WALL) && (g_dx <= 0);
        bool at_left_wall_bounce  = (g_x <= X_LEFT_WALL)  && (g_dx >= 0);
        bool at_wall = at_right_wall_bounce || at_left_wall_bounce;
        bool wall_impact = at_wall && !prev_at_wall &&
                           !skip_first_airborne_wall_flip;
        if (current_switch_on_wall && wall_impact &&
            committed_air_direction != 0)
        {
#ifdef AI_DEBUG
            if (ai_debug) {
                fprintf(stderr, "  air wall-flip: x=%.2f dx=%.2f "
                                "-> air_dir=%+d\n",
                        g_x, g_dx, -committed_air_direction);
            }
#endif
            committed_air_direction = -committed_air_direction;
        }
        skip_first_airborne_wall_flip = false;
        set_ai_key(AI_KEY_LEFT,  committed_air_direction < 0);
        set_ai_key(AI_KEY_RIGHT, committed_air_direction > 0);
        set_ai_key(AI_KEY_JUMP,  false);
        jump_released_since_last_jump = true;
        prev_at_wall = at_wall;
#ifdef AI_DEBUG
        if (ai_debug) sample_ai_time(t_start);
#endif
        return;
    }

#ifdef AI_DEBUG
    /* If we just landed (transitioned from airborne with commit to grounded),
     * log the actual outcome vs. prediction and reset ground-phase stats. */
    if (debug_pending_landing && g_jump_state == JUMP_STATE_IDLE) {
        if (ai_debug) {
            int actual_levels = g_last_level - debug_pre_takeoff_level;
            double dx_error = fabs(g_dx - debug_predicted_land_dx);
            const char *mispredict_tag;
            if (actual_levels != debug_predicted_levels)
                mispredict_tag = "  <-- LEVEL MISPREDICT";
            else if (dx_error > 0.5)
                mispredict_tag = "  <-- DX MISPREDICT";
            else if (actual_levels == 1)
                mispredict_tag = "  <-- SKIP-1!";
            else
                mispredict_tag = "";
            fprintf(stderr,
                "AI landing: actual=%d levels (predicted=%d)"
                " x=%.1f dx=%.2f (predicted_dx=%.2f, err=%.2f)"
                " combo_current=%d(was %d) combo_timer=%d%s\n",
                actual_levels, debug_predicted_levels,
                g_x, g_dx, debug_predicted_land_dx, dx_error,
                g_combo_current, debug_pre_takeoff_combo,
                g_combo_timer, mispredict_tag);
        }
        debug_pending_landing = false;
        /* Reset ground-phase stats for the new grounded interval. */
        debug_ground_ticks = 0;
        debug_ground_flips = 0;
        debug_last_ground_dir = 0;
        debug_ground_start_combo_timer = g_combo_timer;
        debug_ground_start_combo_current = g_combo_current;
    }

    dbg_ptr = ai_debug ? &dbg : NULL;
#endif

    snapshot_state(&s0);
    plan = search_best_plan(&s0, g_rejump
#ifdef AI_DEBUG
                            , dbg_ptr
#endif
                            );

    if (s0.jump_state == JUMP_STATE_IDLE) {
        have_air_commit = false;

        {
            int desired_dir = plan.ground_direction;
            bool at_right_wall_bounce = (g_x >= X_RIGHT_WALL) && (g_dx <= 0);
            bool at_left_wall_bounce  = (g_x <= X_LEFT_WALL)  && (g_dx >= 0);
            bool at_wall = at_right_wall_bounce || at_left_wall_bounce;
            bool wall_impact = at_wall && !prev_at_wall;
            if (current_switch_on_wall && wall_impact &&
                current_ground_direction != 0)
            {
#ifdef AI_DEBUG
                if (ai_debug) {
                    fprintf(stderr, "  ground wall-flip: x=%.2f dx=%.2f "
                                    "ground_dir %+d -> %+d\n",
                            g_x, g_dx,
                            current_ground_direction, -current_ground_direction);
                }
#endif
                desired_dir = -current_ground_direction;
            }

#ifdef AI_DEBUG
            /* Debug: track ground-phase direction flips. */
            if (debug_last_ground_dir != 0 &&
                desired_dir != 0 &&
                desired_dir != debug_last_ground_dir)
            {
                debug_ground_flips++;
                if (ai_debug) {
                    fprintf(stderr, "  ground flip #%d: tick %d, from %+d to %+d, "
                                    "pos=(%.1f,%.1f) dx=%.2f\n",
                        debug_ground_flips, debug_ground_ticks,
                        debug_last_ground_dir, desired_dir,
                        g_x, g_y, g_dx);
                }
            }
            if (desired_dir != 0)
                debug_last_ground_dir = desired_dir;
            debug_ground_ticks++;
#endif

            current_ground_direction = desired_dir;
            current_switch_on_wall = plan.switch_on_wall;

            if (plan.accel_ticks > 0) {
                set_ai_key(AI_KEY_LEFT,  desired_dir < 0);
                set_ai_key(AI_KEY_RIGHT, desired_dir > 0);
                set_ai_key(AI_KEY_JUMP,  false);
                jump_released_since_last_jump = true;
            } else {
                if (jump_released_since_last_jump || g_rejump) {
                    set_ai_key(AI_KEY_LEFT,  desired_dir < 0);
                    set_ai_key(AI_KEY_RIGHT, desired_dir > 0);
                    set_ai_key(AI_KEY_JUMP,  true);
                    jump_released_since_last_jump = false;
                    have_air_commit = true;
                    committed_air_direction = plan.air_direction;
                    /* Suppress spurious wall-flip on next tick if
                     * takeoff-tick wall clamp happens (sim doesn't
                     * treat Phase 2 bounce as switch trigger). */
                    skip_first_airborne_wall_flip = true;

#ifdef AI_DEBUG
                    /* Debug: log takeoff. */
                    if (ai_debug && dbg_ptr) {
                        debug_print_takeoff(&s0, dbg_ptr);
                        debug_pending_landing = true;
                        debug_pre_takeoff_level = g_last_level;
                        debug_predicted_levels =
                            dbg.best_outcome.end_level -
                            dbg.best_outcome.start_level;
                        debug_predicted_land_dx = dbg.best_outcome.landing_dx;
                        debug_pre_takeoff_combo = g_combo_current;
                    }
#endif
                } else {
                    set_ai_key(AI_KEY_LEFT,  desired_dir < 0);
                    set_ai_key(AI_KEY_RIGHT, desired_dir > 0);
                    set_ai_key(AI_KEY_JUMP,  false);
                    jump_released_since_last_jump = true;
                }
            }
        }
    } else {
        have_air_commit = true;
        committed_air_direction = plan.air_direction;
        set_ai_key(AI_KEY_LEFT,  committed_air_direction < 0);
        set_ai_key(AI_KEY_RIGHT, committed_air_direction > 0);
        set_ai_key(AI_KEY_JUMP,  false);
        jump_released_since_last_jump = true;
    }

    {
        bool at_right_wall_bounce = (g_x >= X_RIGHT_WALL) && (g_dx <= 0);
        bool at_left_wall_bounce  = (g_x <= X_LEFT_WALL)  && (g_dx >= 0);
        bool at_wall = at_right_wall_bounce || at_left_wall_bounce;
        prev_at_wall = at_wall;
    }
#ifdef AI_DEBUG
    if (ai_debug) sample_ai_time(t_start);
#endif
}
