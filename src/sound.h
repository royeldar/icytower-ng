#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <stdbool.h>

bool play_sound(const char *path, bool stereo, bool jitter, ALLEGRO_SAMPLE_ID *sample);
