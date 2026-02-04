#pragma once

#include <stdbool.h>

#include "scene.h"

struct shared_state {
    bool fullscreen;
    enum scene scene;
};

void initialize_shared_state(const struct shared_state *shared_state);
struct shared_state *get_shared_state_for_writing();
const struct shared_state *get_shared_state_for_reading();
void write_shared_state();
void read_shared_state();
