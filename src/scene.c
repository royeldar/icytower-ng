/*
 * Scene-related routines
 */

#include <allegro5/allegro.h>
#include <stdbool.h>

#include "exit.h"
#include "menu.h"
#include "scene.h"

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
