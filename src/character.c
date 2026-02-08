/*
 * Character-related routines
 */

#include <allegro5/allegro.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "character.h"
#include "config.h"

#define GAME    "game"
#define CHARACTER   "character"

int g_character = -1;

struct character g_characters[] = {
    {
        .gfx_idle1 = "dave/idle1.bmp",
        .gfx_idle2 = "dave/idle2.bmp",
        .gfx_idle3 = "dave/idle3.bmp",
        .gfx_walk1 = "dave/walk1.bmp",
        .gfx_walk2 = "dave/walk2.bmp",
        .gfx_walk3 = "dave/walk3.bmp",
        .gfx_walk4 = "dave/walk4.bmp",
        .gfx_jump1 = "dave/jump1.bmp",
        .gfx_jump2 = "dave/jump2.bmp",
        .gfx_jump3 = "dave/jump3.bmp",
        .gfx_jump = "dave/jump.bmp",
        .gfx_chock = "dave/chock.bmp",
        .gfx_rotate = "dave/rotate.bmp",
        .gfx_edge1 = "dave/edge1.bmp",
        .gfx_edge2 = "dave/edge2.bmp",
        .sfx_greeting = "dave/yo.ogg",
        .sfx_jumplo = "dave/ho.ogg",
        .sfx_jumpmed = "dave/ahey.ogg",
        .sfx_jumphi = "dave/stayinalive.ogg",
        .sfx_edge = "dave/watchit.ogg",
        .sfx_death = "dave/digging.ogg",
        .sfx_pause = "dave/goinon.ogg",
        .sfx_bgmusic = "dave/bg_dave.ogg",
    },
    {
        .gfx_idle1 = "harold/idle1.bmp",
        .gfx_idle2 = "harold/idle2.bmp",
        .gfx_idle3 = "harold/idle3.bmp",
        .gfx_walk1 = "harold/walk1.bmp",
        .gfx_walk2 = "harold/walk2.bmp",
        .gfx_walk3 = "harold/walk3.bmp",
        .gfx_walk4 = "harold/walk4.bmp",
        .gfx_jump1 = "harold/jump1.bmp",
        .gfx_jump2 = "harold/jump2.bmp",
        .gfx_jump3 = "harold/jump3.bmp",
        .gfx_jump = "harold/jump.bmp",
        .gfx_chock = "harold/chock.bmp",
        .gfx_rotate = "harold/rotate.bmp",
        .gfx_edge1 = "harold/edge1.bmp",
        .gfx_edge2 = "harold/edge2.bmp",
        .sfx_greeting = "harold/yo.ogg",
        .sfx_jumplo = "harold/jump_lo.ogg",
        .sfx_jumpmed = "harold/jump_mid.ogg",
        .sfx_jumphi = "harold/jump_hi.ogg",
        .sfx_edge = "harold/edge.ogg",
        .sfx_death = "harold/falling.ogg",
        .sfx_pause = "harold/wazup.ogg",
        .sfx_bgmusic = "bg_beat.ogg",
    },
};

bool read_character_option() {
    const char *str = al_get_config_value(g_config, GAME, CHARACTER);
    if (str != NULL) {
        int n = atoi(str);
        if (n >= 0 && n < NUM_CHARACTERS)
            g_character = n;
        else
            return false;
    } else {
        g_character = 1;
    }
    return true;
}

void write_character_option() {
    if (g_character != -1) {
        char str[2];
        assert(NUM_CHARACTERS <= 10);
        assert(g_character >= 0 && g_character < NUM_CHARACTERS);
        if (sprintf(str, "%d", g_character) > 0)
            al_set_config_value(g_config, GAME, CHARACTER, str);
    }
}
