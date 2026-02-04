#pragma once

#include <allegro5/allegro.h>

#include "hashmap.h"

struct hashmap *load_resources(const char *subpath, void *(*load_resource)(ALLEGRO_FILE *file, const char *ext), void (*destroy_resource)(void *resource));
void destroy_resources(struct hashmap *map, void (*destroy_resource)(void *resource));
void *get_resource(struct hashmap *map, const char *path);
