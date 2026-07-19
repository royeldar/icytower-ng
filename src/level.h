#pragma once

struct platform {
    int level;
    int pad;
    int start, end;
};

extern struct platform g_platforms[32];

void generate_new_platform();
void generate_platforms();
