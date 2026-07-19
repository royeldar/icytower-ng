#pragma once

#include <stdbool.h>

#include "level.h"

struct shared_state {
    unsigned int frame;
    bool fullscreen;
    int scene;
    unsigned int menu_animation_ticks;
    int heroface_animation_frame;
    int menu_page;
    int menu_item;
    int sound_volume;
    int music_volume;
    int eye_candy;
    int left_key;
    int right_key;
    int jump_key;
    int pause_key;
    bool rejump;
    int character;
    int start_floor;
    struct platform platforms[32];
    int screen_y;
    double x, y;
    double dx, dy;
    int jump_state;
    int edge_state;
    bool spinning;
    double rotation_angle;
    unsigned int gameplay_animation_ticks;
    int character_animation_frame;
    int death;
    bool pause;
    bool escape;
    int combo_timer;
    int combo_current;
    int combo_score;
    int combo_last;
    int combo_best;
    int reward_timer;
    int last_level;
};

void initialize_shared_state(const struct shared_state *shared_state);
struct shared_state *get_shared_state_for_writing();
const struct shared_state *get_shared_state_for_reading();
void write_shared_state();
void read_shared_state();
