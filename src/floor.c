/*
 * Floor-related routines
 */

#include <allegro5/allegro.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "floor.h"

#define GAME    "game"
#define FLOOR   "floor"

int g_start_floor = -1;

struct floor g_floors[] = {
    {
        .left = "floor01.bmp",
        .mid = "floor02.bmp",
        .right = "floor03.bmp",
    },
    {
        .left = "floor04.bmp",
        .mid = "floor05.bmp",
        .right = "floor06.bmp",
    },
    {
        .left = "floor07.bmp",
        .mid = "floor08.bmp",
        .right = "floor09.bmp",
    },
    {
        .left = "floor10.bmp",
        .mid = "floor11.bmp",
        .right = "floor12.bmp",
    },
    {
        .left = "floor12a.bmp",
        .mid = "floor12b.bmp",
        .right = "floor12c.bmp",
    },
    {
        .left = "floor13.bmp",
        .mid = "floor14.bmp",
        .right = "floor15.bmp",
    },
    {
        .left = "floor16.bmp",
        .mid = "floor17.bmp",
        .right = "floor18.bmp",
    },
    {
        .left = "floor18a.bmp",
        .mid = "floor18b.bmp",
        .right = "floor18c.bmp",
    },
    {
        .left = "floor19.bmp",
        .mid = "floor20.bmp",
        .right = "floor21.bmp",
    },
    {
        .left = "floor22.bmp",
        .mid = "floor23.bmp",
        .right = "floor24.bmp",
    },
    {
        .left = "floor25.bmp",
        .mid = "floor26.bmp",
        .right = "floor27.bmp",
    },
};

bool read_start_floor_option() {
    const char *str = al_get_config_value(g_config, GAME, FLOOR);
    if (str != NULL) {
        int n = atoi(str);
        if (n >= 0 && n < NUM_FLOORS)
            g_start_floor = n;
        else
            return false;
    } else {
        g_start_floor = 0;
    }
    return true;
}

void write_start_floor_option() {
    if (g_start_floor != -1) {
        char str[3];
        assert(NUM_FLOORS <= 100);
        assert(g_start_floor >= 0 && g_start_floor < NUM_FLOORS);
        if (sprintf(str, "%d", g_start_floor) > 0)
            al_set_config_value(g_config, GAME, FLOOR, str);
    }
}
