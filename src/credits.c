/*
 * Credits-related routines
 */

#include <allegro5/allegro.h>
#include <stdbool.h>

#include "credits.h"
#include "fonts.h"
#include "gfx.h"
#include "keyboard.h"
#include "music.h"
#include "scene.h"
#include "shared_state.h"

#define MAX_TICKS   150

static int ticks;

/**
 * @brief Initialize the credits scene
 */
void initialize_credits() {
    ticks = 0;
}

/**
 * @brief Finalize the credits scene
 */
void finalize_credits() {
}

/**
 * @brief Update the credits scene one tick
 */
void update_credits() {
    ticks++;
    if (is_key_pressed(ALLEGRO_KEY_ESCAPE) || ticks >= MAX_TICKS)
        transition_scene(EXIT_SCENE, 32);
}

/**
 * @brief Draw the credits scene
 */
void draw_credits(const struct shared_state *shared_state) {
    ALLEGRO_BITMAP *title_bg = get_gfx_bitmap("title_bg.bmp");
    ALLEGRO_BITMAP *title = get_gfx_bitmap("title.bmp");
    al_draw_bitmap(title_bg, 0, 0, 0);
    al_draw_bitmap(title, 250, 20, 0);
    al_draw_text(g_font1, al_map_rgb(255, 255, 255),
        320, 300, ALLEGRO_ALIGN_CENTRE,
        "Thanks for playing!");
}
