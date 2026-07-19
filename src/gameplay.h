#pragma once

#include <stdbool.h>

#include "shared_state.h"

void initialize_gameplay();
void update_gameplay();
void draw_gameplay(const struct shared_state *shared_state);
