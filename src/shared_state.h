#pragma once

#include <stdbool.h>

struct shared_state {
    bool fullscreen;
    int scene;
};

void initialize_shared_state(const struct shared_state *shared_state);
struct shared_state *get_shared_state_for_writing();
const struct shared_state *get_shared_state_for_reading();
void write_shared_state();
void read_shared_state();
