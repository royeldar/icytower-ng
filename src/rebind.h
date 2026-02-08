#pragma once

#include <stdbool.h>

#include "shared_state.h"

void initialize_rebind();
void update_rebind();
void draw_rebind(const struct shared_state *shared_state);
