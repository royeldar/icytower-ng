/*
 * Gameplay-related routines
 */

#include <allegro5/allegro.h>

#include "background.h"
#include "gameplay.h"
#include "gfx.h"
#include "shared_state.h"

int g_screen_y;

/**
 * @brief Initialize the gameplay scene
 */
void initialize_gameplay() {
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
    draw_walls(shared_state);
}
