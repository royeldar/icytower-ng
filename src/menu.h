#pragma once

#include "shared_state.h"

enum menu_page {
    MAIN_PAGE,
    OPTIONS_PAGE,
    GAME_OPTIONS_PAGE,
    GFX_OPTIONS_PAGE,
    SOUND_OPTIONS_PAGE,
    CONTROLS_PAGE,
};

extern enum menu_page g_menu_page;
extern int g_menu_item;

void initialize_menu();
void update_menu();
void draw_menu(const struct shared_state *shared_state);
