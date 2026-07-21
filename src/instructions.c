/*
 * Instructions-related routines
 */

#include <allegro5/allegro.h>

#include "instructions.h"
#include "gfx.h"
#include "keyboard.h"
#include "scene.h"
#include "shared_state.h"

/**
 * @brief Initialize the instructions scene
 */
void initialize_instructions() {
}

/**
 * @brief Finalize the instructions scene
 */
void finalize_instructions() {
}

/**
 * @brief Update the instructions scene one tick
 */
void update_instructions() {
    if (is_key_pressed(ALLEGRO_KEY_ESCAPE) || is_key_pressed(ALLEGRO_KEY_ENTER) || is_key_pressed(ALLEGRO_KEY_SPACE))
        transition_scene(MENU_SCENE, 16);
}

/**
 * @brief Draw the instructions scene
 */
void draw_instructions(const struct shared_state *shared_state) {
    ALLEGRO_BITMAP *title_bg = get_gfx_bitmap("title_bg.bmp");
    ALLEGRO_BITMAP *instructions = get_gfx_bitmap("instructions.bmp");
    al_draw_bitmap(title_bg, 0, 0, 0);
    al_draw_bitmap(instructions, 0, 0, 0);
}
