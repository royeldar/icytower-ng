/*
 * Exit-related routines
 */

#include <allegro5/allegro.h>
#include <stdbool.h>

#include "exit.h"
#include "music.h"
#include "scene.h"
#include "shared_state.h"

/**
 * @brief Initialize the exit scene
 *
 * @param quit set to true if the game should quit
 */
void initialize_exit(bool *quit) {
    stop_music();
    *quit = true;
}

/**
 * @brief Finalize the exit scene
 */
void finalize_exit() {
}

/**
 * @brief Update the exit scene one tick
 */
void update_exit() {
}

/**
 * @brief Draw the exit scene
 */
void draw_exit(const struct shared_state *shared_state) {
}
