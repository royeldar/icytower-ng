#pragma once

#include <stdbool.h>

#include "shared_state.h"

void initialize_exit();
void update_exit(bool *quit);
void draw_exit(const struct shared_state *shared_state);
