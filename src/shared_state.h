#pragma once

#include <stdbool.h>

struct shared_state {
    bool fullscreen;
    int scene;
    unsigned int heroface_animation_ticks;
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
};

void initialize_shared_state(const struct shared_state *shared_state);
struct shared_state *get_shared_state_for_writing();
const struct shared_state *get_shared_state_for_reading();
void write_shared_state();
void read_shared_state();
