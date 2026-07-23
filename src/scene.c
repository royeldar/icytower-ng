/*
 * Scene-related routines
 */

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <assert.h>
#include <stdbool.h>

#include "credits.h"
#include "exit.h"
#include "gameover.h"
#include "gameplay.h"
#include "instructions.h"
#include "menu.h"
#include "rebind.h"
#include "scene.h"
#include "shared_state.h"

enum scene g_scene = -1;
unsigned char g_overlay_alpha = 255;

static bool transition = false;
static unsigned char transition_speed;
static enum scene next_scene;

static void initialize_scene(bool *quit) {
    switch (g_scene) {
    case MENU_SCENE:
        initialize_menu();
        break;
    case GAMEPLAY_SCENE:
        initialize_gameplay();
        break;
    case GAMEOVER_SCENE:
        initialize_gameover();
        break;
    case INSTRUCTIONS_SCENE:
        initialize_instructions();
        break;
    case REBIND_SCENE:
        initialize_rebind();
        break;
    case CREDITS_SCENE:
        initialize_credits();
        break;
    case EXIT_SCENE:
        initialize_exit(quit);
        break;
    }
}

static void finalize_scene() {
    switch (g_scene) {
    case MENU_SCENE:
        finalize_menu();
        break;
    case GAMEPLAY_SCENE:
        finalize_gameplay();
        break;
    case GAMEOVER_SCENE:
        finalize_gameover();
        break;
    case INSTRUCTIONS_SCENE:
        finalize_instructions();
        break;
    case REBIND_SCENE:
        finalize_rebind();
        break;
    case CREDITS_SCENE:
        finalize_credits();
        break;
    case EXIT_SCENE:
        finalize_exit();
        break;
    }
}

static void update_transition(bool *quit) {
    if (g_scene != next_scene) {
        unsigned char next_overlay_alpha = g_overlay_alpha + transition_speed;
        if (next_overlay_alpha > g_overlay_alpha) {
            g_overlay_alpha = next_overlay_alpha;
        } else {
            g_overlay_alpha = 255;
            g_scene = next_scene;
            initialize_scene(quit);
        }
    } else if (g_scene == next_scene) {
        unsigned char next_overlay_alpha = g_overlay_alpha - transition_speed;
        if (next_overlay_alpha < g_overlay_alpha) {
            g_overlay_alpha = next_overlay_alpha;
        } else {
            g_overlay_alpha = 0;
            transition = false;
            finalize_scene();
        }
    }
}

/**
 * @brief Update the game scene one tick
 *
 * @param quit set to true if the game should quit
 */
void update_scene(bool *quit) {
    if (!transition) {
        enum scene scene = g_scene;
        switch (scene) {
        case MENU_SCENE:
            update_menu();
            break;
        case GAMEPLAY_SCENE:
            update_gameplay();
            break;
        case GAMEOVER_SCENE:
            update_gameover();
            break;
        case INSTRUCTIONS_SCENE:
            update_instructions();
            break;
        case REBIND_SCENE:
            update_rebind();
            break;
        case CREDITS_SCENE:
            update_credits();
            break;
        case EXIT_SCENE:
            update_credits();
            break;
        }
    } else {
        update_transition(quit);
    }
}

/**
 * @brief Transition to the next game scene
 *
 * @param new_scene new game scene
 * @param speed transition speed (zero means instantaneous)
 */
void transition_scene(enum scene new_scene, unsigned char speed) {
    assert(!transition);
    assert(new_scene != g_scene);
    if (speed > 0) {
        transition = true;
        transition_speed = speed;
        next_scene = new_scene;
    } else {
        bool quit = false;
        assert(new_scene != EXIT_SCENE);
        g_overlay_alpha = 0;
        g_scene = new_scene;
        initialize_scene(&quit);
        assert(!quit);
    }
}

static void draw_overlay(const struct shared_state *shared_state) {
    unsigned char overlay_alpha = shared_state->overlay_alpha;
    if (overlay_alpha > 0)
        al_draw_filled_rectangle(0, 0, 639, 479, al_map_rgba(0, 0, 0, overlay_alpha));
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
    case GAMEPLAY_SCENE:
        draw_gameplay(shared_state);
        break;
    case GAMEOVER_SCENE:
        draw_gameover(shared_state);
        break;
    case INSTRUCTIONS_SCENE:
        draw_instructions(shared_state);
        break;
    case REBIND_SCENE:
        draw_rebind(shared_state);
        break;
    case CREDITS_SCENE:
        draw_credits(shared_state);
        break;
    case EXIT_SCENE:
        draw_exit(shared_state);
        break;
    }
    draw_overlay(shared_state);
}
