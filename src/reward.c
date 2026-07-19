/*
 * Reward-related routines
 */

#include <allegro5/allegro.h>
#include <assert.h>

#include "reward.h"

#define NUM_REWARDS 10

static struct reward rewards[NUM_REWARDS] = {
    {
        .gfx = "reward000.bmp",
        .sfx = "good.ogg",
    },
    {
        .gfx = "reward001.bmp",
        .sfx = "sweet.ogg",
    },
    {
        .gfx = "reward002.bmp",
        .sfx = "great.ogg",
    },
    {
        .gfx = "reward003.bmp",
        .sfx = "super.ogg",
    },
    {
        .gfx = "reward004.bmp",
        .sfx = "wow.ogg",
    },
    {
        .gfx = "reward005.bmp",
        .sfx = "amazing.ogg",
    },
    {
        .gfx = "reward006.bmp",
        .sfx = "extreme.ogg",
    },
    {
        .gfx = "reward007.bmp",
        .sfx = "fantastic.ogg",
    },
    {
        .gfx = "reward008.bmp",
        .sfx = "splendid.ogg",
    },
    {
        .gfx = "reward009.bmp",
        .sfx = "unbelievable.ogg",
    },
};

struct reward *get_reward(int total) {
    unsigned int i;
    if (total < 7)
        i = 0;
    else if (total < 15)
        i = 1;
    else if (total < 25)
        i = 2;
    else if (total < 35)
        i = 3;
    else if (total < 50)
        i = 4;
    else if (total < 70)
        i = 5;
    else if (total < 100)
        i = 6;
    else if (total < 140)
        i = 7;
    else if (total < 200)
        i = 8;
    else
        i = 9;
    assert(i < NUM_REWARDS);
    return &rewards[i];
}
