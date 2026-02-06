#pragma once

#include <stdbool.h>

#define DEFAULT_EYE_CANDY   2
#define MAX_EYE_CANDY   2

extern int g_eye_candy;

bool read_eye_candy_option();
void write_eye_candy_option();
