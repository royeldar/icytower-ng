#pragma once

#include <allegro5/allegro_audio.h>
#include <stdbool.h>

bool load_sfx_samples(const char *path);
void destroy_sfx_samples();
ALLEGRO_SAMPLE *get_sfx_sample(const char *path);
