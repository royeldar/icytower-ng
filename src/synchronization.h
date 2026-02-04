#pragma once

#include <allegro5/allegro.h>
#include <stdatomic.h>
#include <stdbool.h>

extern ALLEGRO_MUTEX *g_mutex;
extern ALLEGRO_COND *g_cond;

void send_event(int event);
int wait_for_acknowledgement();
int receive_event();
void acknowledge_event(int status);
