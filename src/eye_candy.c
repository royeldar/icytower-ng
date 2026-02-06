#include <allegro5/allegro.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "eye_candy.h"

#define GFX "gfx"
#define EYE_CANDY   "eye_candy"

int g_eye_candy = -1;

bool read_eye_candy_option() {
    const char *str = al_get_config_value(g_config, GFX, EYE_CANDY);
    if (str != NULL) {
        int n = atoi(str);
        if (n >= 0 && n <= 2)
            g_eye_candy = n;
        else
            return false;
    } else {
        g_eye_candy = DEFAULT_EYE_CANDY;
    }
    return true;
}

void write_eye_candy_option() {
    if (g_eye_candy != -1) {
        char str[2];
        assert(MAX_EYE_CANDY <= 9);
        assert(g_eye_candy >= 0 && g_eye_candy <= MAX_EYE_CANDY);
        if (sprintf(str, "%d", g_eye_candy) > 0)
            al_set_config_value(g_config, GFX, EYE_CANDY, str);
    }
}
