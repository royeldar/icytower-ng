/*
 * Graphics assets loading routines
 */

#include <allegro5/allegro.h>
#include <assert.h>
#include <stdbool.h>

#include "gfx.h"
#include "resources.h"

static struct hashmap *gfx_bitmaps = NULL;

static void *load_gfx_bitmap(ALLEGRO_FILE *file, const char *ext) {
    return al_load_bitmap_f(file, ext);
}

static void destroy_gfx_bitmap(void *resource) {
    ALLEGRO_BITMAP *bitmap = resource;
    al_destroy_bitmap(bitmap);
}

/**
 * @brief Load all bitmaps from a given directory
 *
 * @param path relative path to the directory
 * @return true if all bitmaps were loaded successfully
 * @return false otherwise
 */
bool load_gfx_bitmaps(const char *path) {
    assert(gfx_bitmaps == NULL);
    gfx_bitmaps = load_resources(path, load_gfx_bitmap, destroy_gfx_bitmap);
    return (gfx_bitmaps != NULL);
}

/**
 * @brief Destroy all bitmaps
 */
void destroy_gfx_bitmaps() {
    if (gfx_bitmaps != NULL) {
        destroy_resources(gfx_bitmaps, destroy_gfx_bitmap);
        gfx_bitmaps = NULL;
    }
}

/**
 * @brief Get the bitmap corresponding to the given filename
 *
 * @param path filename (relative to the directory the bitmaps were loaded from)
 * @return bitmap corresponding to the given filename (NULL if none)
 */
ALLEGRO_BITMAP *get_gfx_bitmap(const char *path) {
    assert(gfx_bitmaps != NULL);
    return get_resource(gfx_bitmaps, path);
}

/**
 * @brief Iterate on all bitmaps and call a callback on each one
 *
 * @param callback callback which is called for every bitmap
 * @return true if iteration was done successfully
 * @return false otherwise
 */
bool iterate_gfx_bitmaps(bool (*callback)(ALLEGRO_BITMAP *bitmap)) {
    struct hashmap_iter *iter = NULL;
    struct hashmap_entry *entry = NULL;
    assert(gfx_bitmaps != NULL);
    iter = hashmap_iter_create(gfx_bitmaps);
    if (iter == NULL)
        return false;
    while ((entry = hashmap_iter_next(iter))) {
        ALLEGRO_BITMAP *bitmap = *hashmap_entry_get_value_ptr(entry);
        if (!callback(bitmap)) {
            hashmap_iter_destroy(iter);
            return false;
        }
    }
    hashmap_iter_destroy(iter);
    return true;
}
