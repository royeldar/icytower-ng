#pragma once

#include <allegro5/allegro.h>
#include <stdbool.h>

extern ALLEGRO_EVENT_QUEUE *g_event_queue;
extern ALLEGRO_TIMER *g_timer;

bool game_setup();
void game_loop();
void game_cleanup();
