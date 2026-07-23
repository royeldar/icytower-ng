/*
 * Audio assets loading routines
 */

#include <allegro5/allegro_audio.h>
#include <assert.h>
#include <stdbool.h>

#include "sfx.h"
#include "resources.h"

static struct hashmap *sfx_samples = NULL;

static void *load_sfx_sample(ALLEGRO_FILE *file, const char *ext) {
    return al_load_sample_f(file, ext);
}

static void destroy_sfx_sample(void *resource) {
    ALLEGRO_SAMPLE *sample = resource;
    al_destroy_sample(sample);
}

/**
 * @brief Load all audio samples from a given directory
 *
 * @param path relative path to the directory
 * @return true if all audio samples were loaded successfully
 * @return false otherwise
 */
bool load_sfx_samples(const char *path) {
    assert(sfx_samples == NULL);
    sfx_samples = load_resources(path, load_sfx_sample, destroy_sfx_sample);
    return (sfx_samples != NULL);
}

/**
 * @brief Destroy all audio samples
 */
void destroy_sfx_samples() {
    if (sfx_samples != NULL) {
        destroy_resources(sfx_samples, destroy_sfx_sample);
        sfx_samples = NULL;
    }
}

/**
 * @brief Get the audio sample corresponding to the given filename
 *
 * @param path filename (relative to the directory the audio samples were loaded from)
 * @return audio sample corresponding to the given filename (NULL if none)
 */
ALLEGRO_SAMPLE *get_sfx_sample(const char *path) {
    ALLEGRO_SAMPLE *sfx;
    assert(sfx_samples != NULL);
    sfx = get_resource(sfx_samples, path);
    assert(sfx != NULL);
    return sfx;
}
