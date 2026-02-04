/*
 * Exit-related routines
 */

#include <allegro5/allegro.h>
#include <stdbool.h>

#include "exit.h"
#include "fonts.h"
#include "gfx.h"
#include "keyboard.h"
#include "music.h"
#include "scene.h"
#include "shared_state.h"

#define MAX_TICKS   150

static int ticks;

/**
 * @brief Initialize the exit scene
 */
void initialize_exit() {
    ticks = 0;
}

/**
 * @brief Update the exit scene one tick
 *
 * @param quit set to true if the game should quit
 */
void update_exit(bool *quit) {
    ticks++;
    if (is_key_pressed(ALLEGRO_KEY_ESCAPE) || ticks >= MAX_TICKS) {
        stop_music();
        *quit = true;
    }
}

/**
 * @brief Draw the exit scene
 */
void draw_exit(const struct shared_state *shared_state) {
    ALLEGRO_BITMAP *title_bg = get_gfx_bitmap("title_bg.bmp");
    ALLEGRO_BITMAP *title = get_gfx_bitmap("title.bmp");
    al_draw_bitmap(title_bg, 0, 0, 0);
    al_draw_bitmap(title, 250, 20, 0);
    al_draw_text(g_font1, al_map_rgb(255, 255, 255),
        320, 300, ALLEGRO_ALIGN_CENTRE,
        "Thanks for playing!");
}
