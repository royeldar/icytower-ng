#pragma once

#include <stdbool.h>

enum scene {
    MENU_SCENE,
    EXIT_SCENE,
};

extern enum scene g_scene;

void initialize_scene();
void update_scene(bool *quit);
