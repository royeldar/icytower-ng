#pragma once

#include <stdbool.h>

#define NUM_CHARACTERS  2

struct character {
    const char *gfx_idle1;
    const char *gfx_idle2;
    const char *gfx_idle3;
    const char *gfx_walk1;
    const char *gfx_walk2;
    const char *gfx_walk3;
    const char *gfx_walk4;
    const char *gfx_jump1;
    const char *gfx_jump2;
    const char *gfx_jump3;
    const char *gfx_jump;
    const char *gfx_chock;
    const char *gfx_rotate;
    const char *gfx_edge1;
    const char *gfx_edge2;
    const char *sfx_greeting;
    const char *sfx_jumplo;
    const char *sfx_jumpmed;
    const char *sfx_jumphi;
    const char *sfx_edge;
    const char *sfx_death;
    const char *sfx_pause;
    const char *sfx_bgmusic;
};

extern int g_character;

extern struct character g_characters[NUM_CHARACTERS];

bool read_character_option();
void write_character_option();
