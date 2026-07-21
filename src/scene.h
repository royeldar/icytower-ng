#pragma once

#include <stdbool.h>

#include "shared_state.h"

enum scene {
    MENU_SCENE,
    GAMEPLAY_SCENE,
    GAMEOVER_SCENE,
    INSTRUCTIONS_SCENE,
    REBIND_SCENE,
    CREDITS_SCENE,
    EXIT_SCENE,
};

extern enum scene g_scene;
extern unsigned char g_overlay_alpha;

void transition_scene(enum scene new_scene, unsigned char speed);
void update_scene(bool *quit);
void draw_scene(const struct shared_state *shared_state);
