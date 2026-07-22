#pragma once

struct star {
    int timer;
    unsigned int color;
    double x, y;
    double dx, dy;
};

extern struct star g_stars[512];

void initialize_stars();
struct star *create_star(double x, double y);
void update_stars();
const char *get_star_gfx(unsigned int color);
