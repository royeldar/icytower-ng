/*
 * Rebind-related routines
 */

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <assert.h>

#include "controls.h"
#include "fonts.h"
#include "keyboard.h"
#include "menu.h"
#include "rebind.h"
#include "scene.h"
#include "shared_state.h"
#include "sound.h"

static int *key;

/**
 * @brief Initialize the rebind scene
 */
void initialize_rebind() {
    key = NULL;
    switch (g_menu_item) {
    case 0:
        key = &g_left_key;
        break;
    case 1:
        key = &g_right_key;
        break;
    case 2:
        key = &g_jump_key;
        break;
    case 3:
        key = &g_pause_key;
        break;
    }
    assert(key != NULL);
}

/**
 * @brief Finalize the rebind scene
 */
void finalize_rebind() {
}

/**
 * @brief Update the rebind scene one tick
 */
void update_rebind() {
    if (is_key_pressed(ALLEGRO_KEY_ESCAPE)) {
        play_sound("menu_change.ogg", false, false, NULL);
        transition_scene(MENU_SCENE, 0);
    } else {
        int keycode;
        for (keycode = 1; keycode < ALLEGRO_KEY_MAX; ++keycode) {
            if (is_key_pressed(keycode))
                break;
        }
        if (keycode != ALLEGRO_KEY_MAX) {
            play_sound("menu_change.ogg", false, false, NULL);
            *key = keycode;
            transition_scene(MENU_SCENE, 0);
        }
    }
}

/**
 * @brief Draw the rebind scene
 */
void draw_rebind(const struct shared_state *shared_state) {
    char *name = NULL;
    int menu_item = shared_state->menu_item;
    switch (menu_item) {
    case 0:
        name = "Left";
        break;
    case 1:
        name = "Right";
        break;
    case 2:
        name = "Jump";
        break;
    case 3:
        name = "Pause";
        break;
    }
    assert(name != NULL);
    draw_menu(shared_state);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ZERO, ALLEGRO_ALPHA);
    al_draw_filled_rectangle(0, 200, 639, 260, al_map_rgba(0, 0, 0, 200));
    al_draw_filled_rectangle(0, 201, 639, 259, al_map_rgba(0, 0, 0, 195));
    al_draw_filled_rectangle(0, 202, 639, 258, al_map_rgba(0, 0, 0, 190));
    al_draw_filled_rectangle(0, 203, 639, 257, al_map_rgba(0, 0, 0, 185));
    al_draw_filled_rectangle(0, 204, 639, 256, al_map_rgba(0, 0, 0, 180));
    al_draw_filled_rectangle(0, 205, 639, 255, al_map_rgba(0, 0, 0, 175));
    al_draw_filled_rectangle(0, 206, 639, 254, al_map_rgba(0, 0, 0, 170));
    al_draw_filled_rectangle(0, 207, 639, 253, al_map_rgba(0, 0, 0, 165));
    al_draw_filled_rectangle(0, 208, 639, 252, al_map_rgba(0, 0, 0, 160));
    al_draw_filled_rectangle(0, 209, 639, 251, al_map_rgba(0, 0, 0, 155));
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
    al_draw_textf(g_font1, al_map_rgb(255, 255, 255),
        320, 210, ALLEGRO_ALIGN_CENTER,
        "press key for %s", name);
}
