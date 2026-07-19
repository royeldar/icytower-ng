#pragma once

#include <stdbool.h>

#include "shared_state.h"

extern int g_screen_y;

void initialize_gameplay();
void update_gameplay();
void draw_gameplay(const struct shared_state *shared_state);
