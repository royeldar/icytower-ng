#pragma once

#define NUM_REWARDS 10

struct reward {
    const char *gfx;
    const char *sfx;
};

extern struct reward g_rewards[NUM_REWARDS];

unsigned int get_reward(int total);
