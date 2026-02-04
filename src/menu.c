/*
 * Menu-related routines
 */

#include <allegro5/allegro.h>
#include <stdbool.h>

#include "gfx.h"
#include "keyboard.h"
#include "menu.h"
#include "music.h"
#include "scene.h"
#include "shared_state.h"
#include "sound.h"

/**
 * @brief Initialize the menu scene
 */
void initialize_menu() {
    play_music("bg_menu.ogg");
}

/**
 * @brief Update the menu scene one tick
 */
void update_menu() {
    if (is_key_pressed(ALLEGRO_KEY_ESCAPE)) {
        play_sound("menu_change.ogg");
        g_scene = EXIT_SCENE;
    }
}

/**
 * @brief Draw the menu scene
 *
 * @param shared_state shared state
 */
void draw_menu(const struct shared_state *shared_state) {
    ALLEGRO_BITMAP *title_bg = get_gfx_bitmap("title_bg.bmp");
    ALLEGRO_BITMAP *title = get_gfx_bitmap("title.bmp");
    al_draw_bitmap(title_bg, 0, 0, 0);
    al_draw_bitmap(title, 250, 20, 0);
}
