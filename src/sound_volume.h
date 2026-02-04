#pragma once

#include <stdbool.h>

#define DEFAULT_SOUND_VOLUME    6
#define MAX_SOUND_VOLUME    10

extern int g_sound_volume;

bool read_sound_volume_option();
void write_sound_volume_option();
