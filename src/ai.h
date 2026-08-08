#pragma once

#include <stdbool.h>

enum ai_key {
    AI_KEY_LEFT,
    AI_KEY_RIGHT,
    AI_KEY_JUMP,
    AI_KEY_MAX,
};

void do_ai_tick();
void clear_ai_state();
void update_ai();
