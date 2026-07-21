#pragma once

#include <stdbool.h>

#include "shared_state.h"

void initialize_instructions();
void finalize_instructions();
void update_instructions();
void draw_instructions(const struct shared_state *shared_state);
