/*
 * Scene-related routines
 */

#include <allegro5/allegro.h>
#include <stdbool.h>

#include "exit.h"
#include "menu.h"
#include "scene.h"
#include "shared_state.h"

enum scene g_scene = MENU_SCENE;

/**
 * @brief Initialize the game scene
 */
void initialize_scene() {
    switch (g_scene) {
    case MENU_SCENE:
        initialize_menu();
        break;
    case EXIT_SCENE:
        initialize_exit();
        break;
    }
}

/**
 * @brief Update the game scene one tick
 *
 * @param quit set to true if the game should quit
 */
void update_scene(bool *quit) {
    enum scene scene = g_scene;
    switch (scene) {
    case MENU_SCENE:
        update_menu();
        break;
    case EXIT_SCENE:
        update_exit(quit);
        break;
    }
    if (g_scene != scene)
        initialize_scene();
}

/**
 * @brief Draw the game scene
 *
 * @param shared_state shared state
 */
void draw_scene(const struct shared_state *shared_state) {
    enum scene scene = (enum scene)shared_state->scene;
    switch (scene) {
    case MENU_SCENE:
        draw_menu(shared_state);
        break;
    case EXIT_SCENE:
        draw_exit(shared_state);
        break;
    }
}
