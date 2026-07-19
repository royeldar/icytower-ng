#pragma once

struct reward {
    const char *gfx;
    const char *sfx;
};

struct reward *get_reward(int total);
