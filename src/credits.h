#pragma once

#include <stdbool.h>

#include "shared_state.h"

void initialize_credits();
void finalize_credits();
void update_credits();
void draw_credits(const struct shared_state *shared_state);
