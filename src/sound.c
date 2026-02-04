/*
 * Sound-related routines
 */

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <stdbool.h>

#include "sfx.h"
#include "sound_volume.h"

/**
 * @brief Play the audio sample corresponding to the given filename
 *
 * @param path filename (relative to the directory the audio samples were loaded from)
 * @return true if the audio sample was played successfully
 * @return false otherwise
 */
bool play_sound(const char *path) {
    return al_play_sample(get_sfx_sample(path),
        (float)g_sound_volume / (float)MAX_SOUND_VOLUME, 0.0, 1.0,
        ALLEGRO_PLAYMODE_ONCE, NULL);
}
