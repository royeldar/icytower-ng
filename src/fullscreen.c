/*
 * Fullscreen-related routines
 */

#include <allegro5/allegro.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"
#include "fullscreen.h"

#define FULLSCREEN  "fullscreen"

int g_fullscreen = -1;

bool read_fullscreen_option() {
    const char *str = al_get_config_value(g_config, NULL, FULLSCREEN);
    if (str != NULL) {
        switch (atoi(str)) {
        case 1:
            g_fullscreen = 1;
            break;
        case 0:
            g_fullscreen = 0;
            break;
        default:
            return false;
        }
    } else {
        g_fullscreen = 0;
    }
    return true;
}

void write_fullscreen_option() {
    if (g_fullscreen != -1) {
        const char *str = g_fullscreen ? "1" : "0";
        al_set_config_value(g_config, NULL, FULLSCREEN, str);
    }
}
