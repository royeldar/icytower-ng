/*
 * Music-related routines
 */

#define ALLEGRO_UNSTABLE
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <assert.h>
#include <stdbool.h>

#include "music_volume.h"
#include "sfx.h"

static ALLEGRO_SAMPLE_ID music_id;
static bool music_playing = false;

/**
 * @brief Play the audio sample corresponding to the given filename in a loop, if none is currently playing
 *
 * @param path filename (relative to the directory the audio samples were loaded from)
 * @return true if the audio sample was played successfully
 * @return false otherwise
 */
bool play_music(const char *path) {
    if (!music_playing) {
        music_playing = al_play_sample(get_sfx_sample(path),
            (float)g_music_volume / (float)MAX_MUSIC_VOLUME, 0.0, 1.0,
            ALLEGRO_PLAYMODE_LOOP, &music_id);
    }
    return music_playing;
}

/**
 * @brief Stop the audio sample currently playing, if any
 */
void stop_music() {
    if (music_playing) {
        al_stop_sample(&music_id);
        music_playing = false;
    }
}

/**
 * @brief Update the volume of the audio sample instance currently playing, if any
 */
void update_music() {
    ALLEGRO_SAMPLE_INSTANCE *instance = NULL;
    if (music_playing) {
        instance = al_lock_sample_id(&music_id);
    }
    if (instance) {
        al_set_sample_instance_gain(instance, (float)g_music_volume / (float)MAX_MUSIC_VOLUME);
        al_unlock_sample_id(&music_id);
    }
}
