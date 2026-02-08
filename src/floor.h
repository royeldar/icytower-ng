#pragma once

#include <stdbool.h>

#define NUM_FLOORS  11

struct floor {
    const char *left;
    const char *mid;
    const char *right;
};

extern int g_start_floor;

extern struct floor g_floors[NUM_FLOORS];

bool read_start_floor_option();
void write_start_floor_option();
