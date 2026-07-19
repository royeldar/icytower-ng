/*
 * Sound-related routines
 */

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <stdbool.h>

#include "gameplay.h"
#include "random.h"
#include "sfx.h"
#include "sound_volume.h"

/**
 * @brief Play the audio sample corresponding to the given filename
 *
 * @param path filename (relative to the directory the audio samples were loaded from)
 * @param stereo true if the audio should be directional
 * @param jitter true if the speed should be altered
 * @param sample sample id representing the audio sample being played (can be NULL)
 * @return true if the audio sample was played successfully
 * @return false otherwise
 */
bool play_sound(const char *path, bool stereo, bool jitter, ALLEGRO_SAMPLE_ID *sample) {
    float pan = 0.0;
    float speed = 1.0;
    if (stereo)
        pan = 1.5 * (g_x / 640.0) - 0.75;
    if (jitter)
        speed = 0.9 + (rand_custom() % 40000) / 100000.0;
    return al_play_sample(get_sfx_sample(path),
        (float)g_sound_volume / (float)MAX_SOUND_VOLUME, pan, speed,
        ALLEGRO_PLAYMODE_ONCE, sample);
}
