/*
 * Gameplay-related routines
 */

#include <allegro5/allegro.h>

#include "background.h"
#include "floor.h"
#include "fonts.h"
#include "gameplay.h"
#include "gfx.h"
#include "level.h"
#include "random.h"
#include "shared_state.h"

static unsigned int seed;

int g_screen_y;

/**
 * @brief Initialize the gameplay scene
 */
void initialize_gameplay() {
    srand_custom(rand_msvc() % 102392);
    seed = rand_msvc();
    srand_msvc(seed);
    generate_platforms();
    g_screen_y = 0;
}

/**
 * @brief Update the gameplay scene one tick
 */
void update_gameplay() {
    // TODO
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

static void draw_walls(const struct shared_state *shared_state) {
    ALLEGRO_BITMAP *sideblock = get_gfx_bitmap("sideblock.bmp");
    int screen_y = shared_state->screen_y;
    int i;
    for (i = 0; i < 5; ++i) {
        al_draw_bitmap(sideblock, 565, (int)((screen_y % 84) * 1.476) + (i - 1) * 124, 0);
        al_draw_bitmap(sideblock, -57, (int)((screen_y % 84) * 1.476) + (i - 1) * 124, ALLEGRO_FLIP_HORIZONTAL);
    }
}

/**
 * @brief Draw the gameplay scene
 */
void draw_gameplay(const struct shared_state *shared_state) {
    draw_background(shared_state);
    draw_platforms(shared_state);
    draw_walls(shared_state);
}
