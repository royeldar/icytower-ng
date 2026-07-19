#pragma once

#include <stdbool.h>

#include "shared_state.h"

enum jump_state {
    JUMP_STATE_IDLE,
    JUMP_STATE_FLY_UP,
    JUMP_STATE_FLY_DOWN,
    JUMP_STATE_FLY_DOWN_2,
};

enum edge_state {
    EDGE_STATE_IDLE,
    EDGE_STATE_LEFT,
    EDGE_STATE_RIGHT,
};

extern int g_screen_y;
extern double g_x, g_y;
extern double g_dx, g_dy;
extern enum jump_state g_jump_state;
extern enum edge_state g_edge_state;
extern bool g_spinning;
extern double g_rotation_angle;
extern unsigned int g_gameplay_animation_ticks;
extern int g_character_animation_frame;
extern int g_death;
extern bool g_pause;
extern bool g_escape;

void initialize_gameplay();
void update_gameplay();
void draw_gameplay(const struct shared_state *shared_state);
