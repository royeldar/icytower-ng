#pragma once

#include <allegro5/allegro.h>
#include <stdbool.h>

bool load_gfx_bitmaps(const char *path);
void destroy_gfx_bitmaps();
ALLEGRO_BITMAP *get_gfx_bitmap(const char *path);
bool iterate_gfx_bitmaps(bool (*callback)(ALLEGRO_BITMAP *bitmap));
