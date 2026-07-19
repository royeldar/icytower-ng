/*
 * Background-related routines
 */

#include <allegro5/allegro.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "background.h"
#include "gfx.h"
#include "random.h"

#define MAX(a, b)   ((a) > (b) ? (a) : (b))
#define MIN(a, b)   ((a) < (b) ? (a) : (b))

static ALLEGRO_BITMAP *background_bitmap = NULL;
static ALLEGRO_BITMAP *tmp_bitmap = NULL;

// taken from bgtile.bmp
static const unsigned char palette[16][3] = {
    { 0x00, 0x00, 0x00 },
    { 0x0c, 0x0c, 0x14 },
    { 0x18, 0x18, 0x2c },
    { 0x24, 0x24, 0x41 },
    { 0x34, 0x34, 0x55 },
    { 0x45, 0x45, 0x65 },
    { 0x55, 0x55, 0x75 },
    { 0x65, 0x65, 0x86 },
    { 0x75, 0x75, 0x9a },
    { 0x86, 0x86, 0xaa },
    { 0x9a, 0x9a, 0xba },
    { 0xaa, 0xaa, 0xcb },
    { 0xba, 0xba, 0xdf },
    { 0xcb, 0xcb, 0xef },
    { 0xdb, 0xdb, 0xff },
    { 0xeb, 0xeb, 0xff },
};

static int color_to_index(const ALLEGRO_COLOR *c) {
    unsigned int i;
    unsigned char r, g, b;
    al_unmap_rgb(*c, &r, &g, &b);
    for (i = 0; i < 16; ++i) {
        if (palette[i][0] == r && palette[i][1] == g && palette[i][2] == b)
            return i;
    }
    // default color
    return 4;
}

static void draw_background_texture() {
    ALLEGRO_BITMAP *bg_tile = get_gfx_bitmap("bgtile.bmp");
    ALLEGRO_BITMAP *bitmap = al_get_target_bitmap();
    ALLEGRO_LOCKED_REGION *locked_region = NULL;
    int bg_tile_width = al_get_bitmap_width(bg_tile);
    int bg_tile_height = al_get_bitmap_height(bg_tile);
    unsigned int i, j;
    for (i = 0; i < 4; ++i) {
        int x = (i % 2 == 0) ? 32 : 0;
        int y = 32 * i;
        for (j = 0; j < 15; ++j) {
            int w = 64 - x / 4;
            al_draw_scaled_bitmap(bg_tile, 0, 0, bg_tile_width, bg_tile_height,
                x - (w / 2) + 320, y, w, 32, 0);
            al_draw_scaled_bitmap(bg_tile, 0, 0, bg_tile_width, bg_tile_height,
                -x - (w / 2) + 320, y, w, 32, 0);
            x += w;
        }
    }
    locked_region = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
    assert(locked_region != NULL);
    for (i = 0; i < 128; ++i) {
        for (j = 0; j < 640; ++j) {
            ALLEGRO_COLOR c = al_get_pixel(bitmap, j, i);
            int wave = round(al_fixtof(al_fixcos(al_ftofix(abs((int)j - 320) / 6))) * 8.0);
            int noise = rand_custom() % 3 - 1;
            int shift = noise - wave;
            int p = color_to_index(&c);
            unsigned char r, g, b;
            p = MAX(0, MIN(15, p + shift));
            r = palette[p][0];
            g = palette[p][1];
            b = palette[p][2];
            c = al_map_rgb(r, g, b);
            al_put_pixel(j, i, c);
        }
    }
    for (i = 32; i < 96; ++i) {
        for (j = 1; j < 639; ++j) {
            static const float weights[3][3] = {
                { 0.1, 0.1, 0.1 },
                { 0.1, 0.2, 0.1 },
                { 0.1, 0.1, 0.1 },
            };
            float r = 0.0, g = 0.0, b = 0.0;
            ALLEGRO_COLOR c;
            unsigned int k, l;
            for (k = i - 1; k <= i + 1; ++k) {
                for (l = j - 1; l <= j + 1; ++l) {
                    c = al_get_pixel(bitmap, l, k);
                    float w = weights[l - j + 1][k - i + 1];
                    r += w * c.r;
                    g += w * c.g;
                    b += w * c.b;
                }
            }
            c = al_map_rgb_f(r, g, b);
            al_put_pixel(j, i, c);
        }
    }
    al_unlock_bitmap(bitmap);
}

bool create_background_bitmap() {
    int flags;
    assert(background_bitmap == NULL);
    assert(tmp_bitmap == NULL);
    flags = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(flags | ALLEGRO_MEMORY_BITMAP);
    background_bitmap = al_create_bitmap(640, 64);
    tmp_bitmap = al_create_bitmap(640, 480);
    al_set_new_bitmap_flags(flags);
    if (background_bitmap == NULL || tmp_bitmap == NULL)
        return false;
    al_set_target_bitmap(tmp_bitmap);
    draw_background_texture();
    al_set_target_bitmap(background_bitmap);
    al_draw_bitmap_region(tmp_bitmap, 0, 32, 640, 64, 0, 0, 0);
    al_convert_bitmap(background_bitmap);
    return true;
}

void destroy_background_bitmap() {
    if (background_bitmap != NULL) {
        al_destroy_bitmap(background_bitmap);
        background_bitmap = NULL;
    }
    if (tmp_bitmap != NULL) {
        al_destroy_bitmap(tmp_bitmap);
        tmp_bitmap = NULL;
    }
}

ALLEGRO_BITMAP *get_background_bitmap() {
    assert(background_bitmap != NULL);
    return background_bitmap;
}
