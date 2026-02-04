#pragma once

#include <allegro5/allegro.h>
#include <stdbool.h>

extern ALLEGRO_CONFIG *g_config;

bool load_config(const char *subpath);
bool save_config(const char *subpath);
void destroy_config();
