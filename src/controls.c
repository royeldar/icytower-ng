/*
 * Controls-related routines
 */

#include <allegro5/allegro.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "controls.h"

#define CONTROLS    "controls"
#define LEFT    "left"
#define RIGHT   "right"
#define JUMP    "jump"
#define PAUSE   "pause"
#define REJUMP  "rejump"

int g_left_key = -1;
int g_right_key = -1;
int g_jump_key = -1;
int g_pause_key = -1;
int g_rejump = -1;

static bool read_key_option(const char *name, int *key, int default_value) {
    const char *str = al_get_config_value(g_config, CONTROLS, name);
    if (str != NULL) {
        int value = atoi(str);
        if (value > 0 && value < ALLEGRO_KEY_MAX)
            *key = value;
        else
            return false;
    } else {
        *key = default_value;
    }
    return true;
}

static bool read_rejump_option() {
    const char *str = al_get_config_value(g_config, CONTROLS, REJUMP);
    if (str != NULL) {
        switch (atoi(str)) {
        case 1:
            g_rejump = 1;
            break;
        case 0:
            g_rejump = 0;
            break;
        default:
            return false;
        }
    } else {
        g_rejump = 1;
    }
    return true;
}

bool read_controls_options() {
    return read_rejump_option() &&
        read_key_option(LEFT, &g_left_key, ALLEGRO_KEY_LEFT) &&
        read_key_option(RIGHT, &g_right_key, ALLEGRO_KEY_RIGHT) &&
        read_key_option(JUMP, &g_jump_key, ALLEGRO_KEY_SPACE) &&
        read_key_option(PAUSE, &g_pause_key, ALLEGRO_KEY_P);
}

static void write_key_option(const char *name, int key) {
    if (key != -1) {
        char str[4];
        assert(ALLEGRO_KEY_MAX <= 999);
        assert(key > 0 && key < ALLEGRO_KEY_MAX);
        if (sprintf(str, "%d", key) > 0)
            al_set_config_value(g_config, CONTROLS, name, str);
    }
}

static void write_rejump_option() {
    if (g_rejump != -1) {
        const char *str = g_rejump ? "1" : "0";
        al_set_config_value(g_config, CONTROLS, REJUMP, str);
    }
}

void write_controls_options() {
    write_key_option(LEFT, g_left_key);
    write_key_option(RIGHT, g_right_key);
    write_key_option(JUMP, g_jump_key);
    write_key_option(PAUSE, g_pause_key);
    write_rejump_option();
}
