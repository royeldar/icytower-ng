#pragma once

#include <allegro5/allegro.h>
#include <stdbool.h>

bool create_background_bitmap();
void destroy_background_bitmap();
ALLEGRO_BITMAP *get_background_bitmap();
