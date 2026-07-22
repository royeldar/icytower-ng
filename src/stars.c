/*
 * Star-related routines
 */

#include <allegro5/allegro.h>
#include <assert.h>
#include <stdlib.h>

#include "random.h"
#include "stars.h"

struct star g_stars[512];

void initialize_stars() {
    memset(g_stars, 0, sizeof(g_stars));
}

struct star *create_star(double x, double y) {
    unsigned int i;
    for (i = 0; i < 512; ++i) {
        if (g_stars[i].timer == 0)
            break;
    }
    if (i < 512) {
        struct star *star = &g_stars[i];
        star->timer = 255;
        star->x = x;
        star->y = y;
        star->dx = (rand_custom() % 50 - 25) / 10.0;
        star->dy = (rand_custom() % 50 - 25) / 50.0;
        star->color = rand_custom() % 8;
        return star;
    } else {
        return &g_stars[0];
    }
}

void update_stars() {
    unsigned int i;
    for (i = 0; i < 512; ++i) {
        struct star *star = &g_stars[i];
        if (star->timer == 0)
            continue;
        star->timer--;
        star->x += star->dx;
        star->y += star->dy;
        star->dy += 0.3;
        if (rand_custom() % 5 == 1)
            star->color = rand_custom() % 8;
    }
}

const char *get_star_gfx(unsigned int color) {
    assert(color < 8);
    switch (color) {
    case 0:
        return "star01.bmp";
    case 1:
        return "star02.bmp";
    case 2:
        return "star03.bmp";
    case 3:
        return "star04.bmp";
    case 4:
        return "star05.bmp";
    case 5:
        return "star06.bmp";
    case 6:
        return "star07.bmp";
    case 7:
        return "star08.bmp";
    default:
        return NULL;
    }
}
