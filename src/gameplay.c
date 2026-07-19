/*
 * Gameplay-related routines
 */

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

#include "background.h"
#include "character.h"
#include "controls.h"
#include "floor.h"
#include "fonts.h"
#include "gameplay.h"
#include "gfx.h"
#include "keyboard.h"
#include "level.h"
#include "music.h"
#include "random.h"
#include "scene.h"
#include "shared_state.h"
#include "sound.h"

static unsigned int seed;

int g_screen_y;

double g_x, g_y;
double g_dx, g_dy;

enum jump_state g_jump_state;
enum edge_state g_edge_state;
bool g_spinning;
double g_rotation_angle;

unsigned int g_gameplay_animation_ticks = 0;
int g_character_animation_frame = 0;

int g_death;

bool g_pause;
bool g_escape;

int g_combo_timer;
int g_combo_current;
int g_combo_score;
int g_combo_last;
int g_combo_best;

int g_reward_timer;

int g_last_level;

static unsigned int edge_ticks = 0;

static bool jumped;

static int prev_x;
static int prev_y;
static int prev_screen_y;

static int jump_streak;

static bool quit;
static bool wait_resume;

/**
 * @brief Initialize the gameplay scene
 */
void initialize_gameplay() {
    srand_custom(rand_msvc() % 102392);
    seed = rand_msvc();
    srand_msvc(seed);
    generate_platforms();
    g_screen_y = 0;
    g_x = 200.0;
    g_y = 431.0;
    g_dx = 0.001;
    g_dy = 0.0;
    g_jump_state = JUMP_STATE_IDLE;
    g_edge_state = EDGE_STATE_IDLE;
    g_spinning = false;
    g_death = 0;
    g_pause = false;
    g_escape = false;
    g_combo_timer = 0;
    g_combo_current = 0;
    g_combo_score = 0;
    g_combo_last = 0;
    g_combo_best = 0;
    g_reward_timer = 0;
    g_last_level = 0;
    jumped = true;
    jump_streak = 0;
    quit = false;
    wait_resume = false;
    play_music(g_characters[g_character].sfx_bgmusic);
    play_sound(g_characters[g_character].sfx_greeting, false, false, NULL);
}

static void update_movement() {
    if (is_key_down(g_left_key)) {
        if (g_dx > 0.0)
            g_dx *= 0.7;
        g_dx -= 0.3;
    } else if (is_key_down(g_right_key)) {
        if (g_dx < 0.0)
            g_dx *= 0.7;
        g_dx += 0.3;
    } else {
        g_dx *= 0.9;
    }
    if (is_key_down(g_jump_key) && (g_rejump || !jumped) && (g_jump_state == JUMP_STATE_IDLE)) {
        jumped = true;
        g_jump_state = JUMP_STATE_FLY_UP;
        g_dy = (g_dx < 0.0) ? (2.0 * g_dx) : (-2.0 * g_dx);
        if (g_dy > -12.2)
            g_dy = -12.2;
        if (g_dy < -22.0) {
            g_spinning = true;
            g_rotation_angle = 0.0;
            play_sound(g_characters[g_character].sfx_jumphi, true, true, NULL);
        } else if (g_dy < -15.0) {
            play_sound(g_characters[g_character].sfx_jumpmed, true, true, NULL);
        } else {
            play_sound(g_characters[g_character].sfx_jumplo, true, true, NULL);
        }
    } else if (!is_key_down(g_jump_key)) {
        jumped = false;
    }
}

static void update_position() {
    prev_x = g_x;
    prev_y = g_y;
    if (g_dx > 12.2)
        g_dx = 12.2;
    else if (g_dx < -12.2)
        g_dx = -12.2;
    if (g_dy > 12.2)
        g_dy = 12.2;
    else if (g_dy < -100.0)
        g_dy = -100.0;
    g_x += g_dx;
    g_y += g_dy;
    if (g_x > 555.0) {
        g_x = 555.0;
        g_dx = g_dx * -0.9;
    } else if (g_x < 85.0) {
        g_x = 85.0;
        g_dx = g_dx * -0.9;
    }
    if (g_jump_state != JUMP_STATE_IDLE) {
        g_dy += 0.8;
        if (g_jump_state == JUMP_STATE_FLY_UP && g_dy > 0.0)
            g_jump_state = JUMP_STATE_FLY_DOWN;
    }
}

static void update_screen() {
    int screen_dy;
    prev_screen_y = g_screen_y;
    if (g_y < 0.0)
        screen_dy = 13;
    else if (g_y < 20.0)
        screen_dy = 10;
    else if (g_y < 40.0)
        screen_dy = 8;
    else if (g_y < 60.0)
        screen_dy = 6;
    else if (g_y < 80.0)
        screen_dy = 5;
    else if (g_y < 100.0)
        screen_dy = 4;
    else if (g_y < 120.0)
        screen_dy = 3;
    else if (g_y < 140.0)
        screen_dy = 2;
    else if (g_y < 160.0)
        screen_dy = 1;
    else
        screen_dy = 0;
    g_screen_y += screen_dy;
    g_y += screen_dy;
    prev_y += screen_dy;
}

static void update_platforms() {
    if ((g_screen_y % 16 < prev_screen_y % 16) || (g_screen_y - prev_screen_y >= 16))
        generate_new_platform();
}

static bool get_platform(int y, int *platform_y, int *platform_x_left, int *platform_x_right, int *platform_level) {
    struct platform *platform;
    unsigned int i = 29 - (y + 1) / 16;
    if (i >= 32)
        return false;
    platform = &g_platforms[i];
    if (platform->pad == 0) {
        *platform_y = g_screen_y % 16 + 16 * (29 - i);
        *platform_x_left = 16 * platform->start - 2;
        *platform_x_right = 16 * platform->end + 17;
        *platform_level = platform->level;
        return true;
    } else {
        return false;
    }
}

static int get_level(int y) {
    struct platform *platform;
    unsigned int i = 29 - (y + 1) / 16;
    if (i >= 32)
        return 0;
    platform = &g_platforms[i];
    return platform->level;
}

static bool line_intersection(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int *x, int *y) {
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

static void update_collisions() {
    int x = g_x;
    int y = g_y;
    int platform_y;
    int platform_x_left, platform_x_right;
    int platform_level;
    bool collision_left, collision_right;
    int collision_x_left, collision_x_right;
    int collision_y_left, collision_y_right;
    if (!get_platform(y, &platform_y, &platform_x_left, &platform_x_right, &platform_level) &&
        !get_platform(prev_y, &platform_y, &platform_x_left, &platform_x_right, &platform_level)) {
        if (g_jump_state == JUMP_STATE_IDLE || g_jump_state == JUMP_STATE_FLY_DOWN)
            g_jump_state = JUMP_STATE_FLY_DOWN_2;
        return;
    }
    collision_left = line_intersection(platform_x_left, platform_y, platform_x_right, platform_y,
        x - 11, y + 1, prev_x - 11, prev_y, &collision_x_left, &collision_y_left);
    collision_right = line_intersection(platform_x_left, platform_y, platform_x_right, platform_y,
        x + 11, y + 1, prev_x + 11, prev_y, &collision_x_right, &collision_y_right);
    if (collision_left != collision_right)
        g_edge_state = collision_left ? EDGE_STATE_RIGHT : EDGE_STATE_LEFT;
    else
        g_edge_state = EDGE_STATE_IDLE;
    if (collision_left || collision_right) {
        if (g_jump_state == JUMP_STATE_FLY_DOWN || g_jump_state == JUMP_STATE_FLY_DOWN_2) {
            if (!collision_left || !collision_right ||
                (collision_x_left >= -10000 && collision_x_right >= -10000 &&
                collision_x_left <= 10000 && collision_x_right <= 10000)) {
                play_sound("step.ogg", true, true, NULL);
                g_jump_state = JUMP_STATE_IDLE;
                g_y = platform_y - 1;
                g_dy = 0.0;
                if (collision_left)
                    g_x = collision_x_left + 11;
                else
                    g_x = collision_x_right - 11;
                g_spinning = false;
            }
        }
    } else {
        if (g_jump_state == JUMP_STATE_IDLE || g_jump_state == JUMP_STATE_FLY_DOWN)
            g_jump_state = JUMP_STATE_FLY_DOWN_2;
    }
}

static void update_score() {
    if (g_combo_timer != 0) {
        g_combo_timer--;
        if (g_combo_timer == 0 && jump_streak >= 2) {
            g_combo_score += g_combo_current * g_combo_current;
            g_combo_last = g_combo_current;
            if (g_combo_current > g_combo_best)
                g_combo_best = g_combo_current;
            g_reward_timer = 80;
        }
    }
    if (g_jump_state == JUMP_STATE_IDLE) {
        int level = get_level(g_y);
        if (level >= g_last_level + 2) {
            if (g_combo_timer) {
                g_combo_current += level - g_last_level;
                jump_streak++;
            } else {
                g_combo_current = level - g_last_level;
                jump_streak = 1;
            }
            g_combo_timer = 100;
        } else if (level != g_last_level) {
            if (g_combo_timer != 0)
                g_combo_timer = 1;
        }
        g_last_level = level;
    }
}

static void update_death() {
    if (g_y > 540.0 && !g_death) {
        g_combo_timer = 0;
        g_death = 1;
        play_sound(g_characters[g_character].sfx_death, true, false, NULL);
    }
}

static void update_edge_sfx() {
    if (g_edge_state == EDGE_STATE_IDLE)
        edge_ticks = 0;
    if (g_jump_state == JUMP_STATE_IDLE && edge_ticks == 11)
        play_sound(g_characters[g_character].sfx_edge, true, true, NULL);
    if (g_edge_state != EDGE_STATE_IDLE)
        if (++edge_ticks == 50)
            edge_ticks = 0;
}

static void update_animations() {
    if (++g_gameplay_animation_ticks == 50)
        g_gameplay_animation_ticks = 0;
    if (g_gameplay_animation_ticks % 10 == 0)
        if (++g_character_animation_frame == 4)
            g_character_animation_frame = 0;
    if (g_spinning)
        g_rotation_angle += al_fixtof(8 * al_fixtorad_r);
    if (g_death && g_death < 300)
        g_death += 8;
    if (g_reward_timer != 0)
        g_reward_timer--;
}

static void update_pause() {
    if (is_key_pressed(ALLEGRO_KEY_ESCAPE)) {
        if (g_death) {
            quit = true;
        } else {
            g_pause = true;
            g_escape = true;
            play_sound(g_characters[g_character].sfx_pause, true, false, NULL);
        }
    } else if (is_key_pressed(g_pause_key) && !g_death) {
        g_pause = true;
        g_escape = false;
        play_sound(g_characters[g_character].sfx_pause, true, false, NULL);
    }
}

static void update_resume() {
    if (wait_resume) {
        if (is_key_released(g_pause_key)) {
            g_pause = false;
            g_escape = false;
            wait_resume = false;
        }
    } else if (is_key_pressed(g_pause_key)) {
        wait_resume = true;
    } else if (g_escape && is_key_pressed(ALLEGRO_KEY_ESCAPE)) {
        stop_music();
        play_sound("tryagain.ogg", false, false, NULL);
        g_scene = MENU_SCENE;
    } else if (is_any_key_pressed()) {
        g_pause = false;
        g_escape = false;
    }
}

/**
 * @brief Update the gameplay scene one tick
 */
void update_gameplay() {
    if (!g_pause) {
        if (!quit) {
            update_movement();
            update_position();
            update_screen();
            update_platforms();
            update_collisions();
            update_score();
            update_death();
            update_edge_sfx();
            update_animations();
            update_pause();
        } else {
            // TODO
        }
    } else {
        update_resume();
    }
}

static void draw_background(const struct shared_state *shared_state) {
    ALLEGRO_BITMAP *background_texture = get_background_bitmap();
    int screen_y = shared_state->screen_y;
    int i;
    for (i = 0; i < 9; ++i) {
        al_draw_bitmap_region(background_texture, 60, 0, 520, 64,
            60, (screen_y % 128) / 2 + (i - 1) * 64, 0);
    }
}

static void draw_platforms(const struct shared_state *shared_state) {
    const struct platform *platforms = shared_state->platforms;
    int start_floor = shared_state->start_floor;
    int screen_y = shared_state->screen_y;
    int i;
    for (i = 31; i >= 0; --i) {
        const struct platform *platform = &platforms[i];
        int floor;
        ALLEGRO_BITMAP *floor_left;
        ALLEGRO_BITMAP *floor_mid;
        ALLEGRO_BITMAP *floor_right;
        int x, y;
        if (platform->pad != 0)
            continue;
        floor = start_floor + platform->level / 100;
        if (floor >= NUM_FLOORS - 1)
            floor = NUM_FLOORS - 2;
        if (platform->level >= (NUM_FLOORS - 1) * 100)
            floor = NUM_FLOORS - 1;
        floor_left = get_gfx_bitmap(g_floors[floor].left);
        floor_mid = get_gfx_bitmap(g_floors[floor].mid);
        floor_right = get_gfx_bitmap(g_floors[floor].right);
        x = 16 * platform->start - 5;
        y = screen_y % 16 - 6 + 16 * (29 - i);
        al_draw_bitmap(floor_left, x, y, 0);
        x += 21;
        while (x < platform->end * 16) {
            al_draw_bitmap(floor_mid, x, y, 0);
            x += 16;
        }
        al_draw_bitmap(floor_right, x, y, 0);
        if (platform->level != 0 && platform->level % 10 == 0) {
            ALLEGRO_BITMAP *floor_sign = get_gfx_bitmap(g_floors[floor].sign);
            int w = al_get_bitmap_width(floor_sign);
            x = 16 * (platform->start + (platform->end - platform->start) / 2);
            y += 16;
            al_draw_bitmap(floor_sign, x, y, 0);
            x += w / 2;
            al_draw_textf(g_font3, al_map_rgb(16, 16, 16),
                x + 1, y + 7, ALLEGRO_ALIGN_CENTER, "%d", platform->level);
            al_draw_textf(g_font3, al_map_rgb(16, 16, 16),
                x + 2, y + 6, ALLEGRO_ALIGN_CENTER, "%d", platform->level);
            al_draw_textf(g_font3, al_map_rgb(16, 16, 16),
                x, y + 6, ALLEGRO_ALIGN_CENTER, "%d", platform->level);
            al_draw_textf(g_font3, al_map_rgb(16, 16, 16),
                x + 1, y + 5, ALLEGRO_ALIGN_CENTER, "%d", platform->level);
            al_draw_textf(g_font3, al_map_rgb(255, 255, 255),
                x + 1, y + 6, ALLEGRO_ALIGN_CENTER, "%d", platform->level);
        }
    }
}

static void draw_character(const struct shared_state *shared_state) {
    int character = shared_state->character;
    ALLEGRO_BITMAP *character_idle1 = get_gfx_bitmap(g_characters[character].gfx_idle1);
    ALLEGRO_BITMAP *character_idle2 = get_gfx_bitmap(g_characters[character].gfx_idle2);
    ALLEGRO_BITMAP *character_idle3 = get_gfx_bitmap(g_characters[character].gfx_idle3);
    ALLEGRO_BITMAP *character_walk1 = get_gfx_bitmap(g_characters[character].gfx_walk1);
    ALLEGRO_BITMAP *character_walk2 = get_gfx_bitmap(g_characters[character].gfx_walk2);
    ALLEGRO_BITMAP *character_walk3 = get_gfx_bitmap(g_characters[character].gfx_walk3);
    ALLEGRO_BITMAP *character_walk4 = get_gfx_bitmap(g_characters[character].gfx_walk4);
    ALLEGRO_BITMAP *character_jump1 = get_gfx_bitmap(g_characters[character].gfx_jump1);
    ALLEGRO_BITMAP *character_jump2 = get_gfx_bitmap(g_characters[character].gfx_jump2);
    ALLEGRO_BITMAP *character_jump3 = get_gfx_bitmap(g_characters[character].gfx_jump3);
    ALLEGRO_BITMAP *character_jump = get_gfx_bitmap(g_characters[character].gfx_jump);
    ALLEGRO_BITMAP *character_chock = get_gfx_bitmap(g_characters[character].gfx_chock);
    ALLEGRO_BITMAP *character_rotate = get_gfx_bitmap(g_characters[character].gfx_rotate);
    ALLEGRO_BITMAP *character_edge1 = get_gfx_bitmap(g_characters[character].gfx_edge1);
    ALLEGRO_BITMAP *character_edge2 = get_gfx_bitmap(g_characters[character].gfx_edge2);
    int screen_y = shared_state->screen_y;
    int x = shared_state->x;
    int y = shared_state->y;
    double dx = shared_state->dx;
    double dy = shared_state->dy;
    enum jump_state jump_state = shared_state->jump_state;
    enum edge_state edge_state = shared_state->edge_state;
    bool spinning = shared_state->spinning;
    double rotation_angle = shared_state->rotation_angle;
    unsigned int gameplay_animation_ticks = shared_state->gameplay_animation_ticks;
    int character_animation_frame = shared_state->character_animation_frame;
    ALLEGRO_BITMAP *character_bitmap = NULL;
    if (jump_state == JUMP_STATE_IDLE) {
        if (fabs(dx) < 0.02) {
            if (edge_state == EDGE_STATE_IDLE) {
                if (screen_y <= 200 || y <= 400.0) {
                    if (gameplay_animation_ticks <= 11)
                        character_bitmap = character_idle2;
                    else if (gameplay_animation_ticks <= 24)
                        character_bitmap = character_idle1;
                    else if (gameplay_animation_ticks <= 36)
                        character_bitmap = character_idle3;
                    else
                        character_bitmap = character_idle1;
                } else {
                    character_bitmap = character_chock;
                }
                if (dx > 0.0)
                    al_draw_bitmap(character_bitmap, x - 14, y - 51, 0);
                else
                    al_draw_bitmap(character_bitmap, x - 14, y - 51, ALLEGRO_FLIP_HORIZONTAL);
            } else if (edge_state == EDGE_STATE_LEFT) {
                if ((gameplay_animation_ticks & 8) == 0)
                    character_bitmap = character_edge1;
                else
                    character_bitmap = character_edge2;
                al_draw_bitmap(character_bitmap, x - 26, y - 50, ALLEGRO_FLIP_HORIZONTAL);
            } else if (edge_state == EDGE_STATE_RIGHT) {
                if ((gameplay_animation_ticks & 8) == 0)
                    character_bitmap = character_edge1;
                else
                    character_bitmap = character_edge2;
                al_draw_bitmap(character_bitmap, x - 11, y - 50, 0);
            }
        } else {
            if (fabs(dx) < 0.2)
                character_animation_frame = 0;
            switch (character_animation_frame) {
            case 0:
                character_bitmap = character_walk1;
                break;
            case 1:
                character_bitmap = character_walk2;
                break;
            case 2:
                character_bitmap = character_walk3;
                break;
            case 3:
                character_bitmap = character_walk4;
                break;
            }
            assert(character_bitmap != NULL);
            if (dx > 0.0)
                al_draw_bitmap(character_bitmap, x - 14, y - 51, 0);
            else
                al_draw_bitmap(character_bitmap, x - 14, y - 51, ALLEGRO_FLIP_HORIZONTAL);
        }
    } else if (spinning) {
        float w = al_get_bitmap_width(character_rotate);
        float h = al_get_bitmap_height(character_rotate);
        al_draw_rotated_bitmap(character_rotate, w / 2, h / 2, x - 14 + w / 2, y - 51 + h / 2, rotation_angle, 0);
    } else {
        if (fabs(dx) < 0.01)
            character_bitmap = character_jump;
        else if (jump_state == JUMP_STATE_FLY_UP && dy < -3.0)
            character_bitmap = character_jump1;
        else if ((jump_state == JUMP_STATE_FLY_DOWN || jump_state == JUMP_STATE_FLY_DOWN_2) && dy > 3.0)
            character_bitmap = character_jump3;
        else
            character_bitmap = character_jump2;
        if (dx > 0.0)
            al_draw_bitmap(character_bitmap, x - 14, y - 51, 0);
        else
            al_draw_bitmap(character_bitmap, x - 14, y - 51, ALLEGRO_FLIP_HORIZONTAL);
    }
}

static void draw_walls(const struct shared_state *shared_state) {
    ALLEGRO_BITMAP *sideblock = get_gfx_bitmap("sideblock.bmp");
    int screen_y = shared_state->screen_y;
    int i;
    for (i = 0; i < 5; ++i) {
        al_draw_bitmap(sideblock, 565, (int)((screen_y % 84) * 1.476) + (i - 1) * 124, 0);
        al_draw_bitmap(sideblock, -57, (int)((screen_y % 84) * 1.476) + (i - 1) * 124, ALLEGRO_FLIP_HORIZONTAL);
    }
}

static void draw_combo(const struct shared_state *shared_state) {
    ALLEGRO_BITMAP *combo_meter = get_gfx_bitmap("combo_meter.bmp");
    ALLEGRO_BITMAP *combo_liquid = get_gfx_bitmap("combo_liquid.bmp");
    ALLEGRO_BITMAP *combo_count = get_gfx_bitmap("combo_count.bmp");
    int combo_timer = shared_state->combo_timer;
    int combo_current = shared_state->combo_current;
    int combo_last = shared_state->combo_last;
    al_draw_bitmap(combo_meter, 20, 100, 0);
    if (combo_timer != 0) {
        al_draw_bitmap_region(combo_liquid, 0, 100 - combo_timer, 16, combo_timer, 31, 219 - combo_timer, 0);
        al_draw_bitmap(combo_count, -10, 210, 0);
        al_draw_textf(g_font1, al_map_rgb(255, 255, 255), 40, 214, ALLEGRO_ALIGN_CENTER, "%d", combo_current);
    } else if (g_reward_timer != 0) {
        al_draw_bitmap(combo_count, -10, 210, 0);
        al_draw_textf(g_font1, al_map_rgb(255, 255, 255), 40, 214, ALLEGRO_ALIGN_CENTER, "%d", combo_last);
    }
}

static void draw_score(const struct shared_state *shared_state) {
    int combo_score = shared_state->combo_score;
    int last_level = shared_state->last_level;
    al_draw_textf(g_font1, al_map_rgb(255, 255, 255),
        8, 440, ALLEGRO_ALIGN_LEFT,
        "score: %d", combo_score + 10 * last_level);
}

static void draw_pause(const struct shared_state *shared_state) {
    bool pause = shared_state->pause;
    bool escape = shared_state->escape;
    if (pause) {
        unsigned int x, y;
        for (x = 0; x < 640; x += 2) {
            al_draw_line(x, 0, x, 480, al_map_rgb(0, 0, 0), 0);
        }
        for (y = 0; y < 480; y += 2) {
            al_draw_line(0, y, 640, y, al_map_rgb(0, 0, 0), 0);
        }
        if (escape) {
            al_draw_text(g_font1, al_map_rgb(255, 255, 255),
                320, 160, ALLEGRO_ALIGN_CENTER,
                "DO YOU REALLY WANT TO EXIT?");
            al_draw_text(g_font2, al_map_rgb(255, 255, 255),
                320, 210, ALLEGRO_ALIGN_CENTER,
                "Press any key to resume");
            al_draw_text(g_font2, al_map_rgb(255, 255, 255),
                320, 240, ALLEGRO_ALIGN_CENTER,
                "Press ESC to exit");
        } else {
            al_draw_text(g_font1, al_map_rgb(255, 255, 255),
                320, 160, ALLEGRO_ALIGN_CENTER,
                "Game Paused");
            al_draw_text(g_font2, al_map_rgb(255, 255, 255),
                320, 210, ALLEGRO_ALIGN_CENTER,
                "Press any key to resume");
        }
    }
}

/**
 * @brief Draw the gameplay scene
 */
void draw_gameplay(const struct shared_state *shared_state) {
    draw_background(shared_state);
    draw_platforms(shared_state);
    draw_character(shared_state);
    draw_walls(shared_state);
    draw_combo(shared_state);
    draw_score(shared_state);
    draw_pause(shared_state);
}
