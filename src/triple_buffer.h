#pragma once

#include <stdatomic.h>
#include <stdbool.h>

struct triple_buffer {
    void *buf[3];
    atomic_int state;
};

void *get_buffer(const struct triple_buffer *triple_buffer, int index);
bool swap_write_buffer(struct triple_buffer *triple_buffer, int *index);
bool swap_read_buffer(struct triple_buffer *triple_buffer, int *index);
