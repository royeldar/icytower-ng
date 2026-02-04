#include <allegro5/allegro.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "sound_volume.h"

#define SFX "sfx"
#define SOUND   "sound"

int g_sound_volume = -1;

bool read_sound_volume_option() {
    const char *str = al_get_config_value(g_config, SFX, SOUND);
    if (str != NULL) {
        int n = atoi(str);
        if (n >= 0 && n <= MAX_SOUND_VOLUME)
            g_sound_volume = n;
        else
            return false;
    } else {
        g_sound_volume = DEFAULT_SOUND_VOLUME;
    }
    return true;
}

void write_sound_volume_option() {
    if (g_sound_volume != -1) {
        char str[3];
        assert(MAX_SOUND_VOLUME <= 99);
        assert(g_sound_volume >= 0 && g_sound_volume <= MAX_SOUND_VOLUME);
        if (sprintf(str, "%d", g_sound_volume) > 0)
            al_set_config_value(g_config, SFX, SOUND, str);
    }
}
