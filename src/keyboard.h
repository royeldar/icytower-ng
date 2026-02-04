#pragma once

#include <allegro5/allegro.h>
#include <stdbool.h>

bool is_key_down(int keycode);
bool is_key_pressed(int keycode);
bool is_key_released(int keycode);
void handle_keyboard_event(const ALLEGRO_KEYBOARD_EVENT *event);
void do_keyboard_tick();
void clear_keyboard_state();
