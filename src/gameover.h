#pragma once

#include "shared_state.h"

extern int g_gameover_item;

void initialize_gameover();
void update_gameover();
void draw_gameover(const struct shared_state *shared_state);
