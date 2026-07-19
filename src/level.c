/*
 * Level-related routines
 */

#include <allegro5/allegro.h>
#include <assert.h>

#include "level.h"
#include "random.h"

struct platform g_platforms[32];

void generate_new_platform() {
    memmove(g_platforms, g_platforms + 1, sizeof(g_platforms) - sizeof(struct platform));
    if (g_platforms[31].pad == 4) {
        int level = ++g_platforms[31].level;
        assert(level >= 0);
        g_platforms[31].pad = 0;
        if (level <= 1000 ? (level % 50 == 0) : (level % 500 == 0)) {
            g_platforms[31].start = 0;
            g_platforms[31].end = 40;
        } else {
            int start;
            int length;
            if (level < 600) {
                long double divisor = (long double)(300 - level) * 0.03333333333333333;
                length = 6 + rand_msvc() % (int)(divisor >= 1.0 ? divisor : 1.0);
            } else if (level <= 1000) {
                length = 6;
            } else if (level <= 1500) {
                length = 5;
            } else if (level <= 2000) {
                length = 4;
            } else if (level <= 10000) {
                length = 3;
            } else {
                length = 2;
            }
            start = 5 + rand_msvc() % (30 - length);
            g_platforms[31].start = start;
            g_platforms[31].end = start + length;
        }
    } else {
        g_platforms[31].pad++;
    }
}

void generate_platforms() {
    int i;
    g_platforms[30].pad = 3;
    g_platforms[30].level = -1;
    g_platforms[31].pad = 4;
    g_platforms[31].level = -1;
    for (i = 0; i < 30; ++i) {
        generate_new_platform();
    }
}
