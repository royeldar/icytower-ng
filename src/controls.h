#pragma once

#include <stdbool.h>

extern int g_left_key;
extern int g_right_key;
extern int g_jump_key;
extern int g_pause_key;
extern int g_rejump;

bool read_controls_options();
void write_controls_options();
