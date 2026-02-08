#pragma once

#include <stdbool.h>

#include "shared_state.h"

enum scene {
    MENU_SCENE,
    INSTRUCTIONS_SCENE,
    REBIND_SCENE,
    EXIT_SCENE,
};

extern enum scene g_scene;

void initialize_scene();
void update_scene(bool *quit);
void draw_scene(const struct shared_state *shared_state);
