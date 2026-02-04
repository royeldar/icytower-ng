#include <allegro5/allegro.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "music_volume.h"

#define SFX "sfx"
#define MUSIC   "music"

int g_music_volume = -1;

bool read_music_volume_option() {
    const char *str = al_get_config_value(g_config, SFX, MUSIC);
    if (str != NULL) {
        int n = atoi(str);
        if (n >= 0 && n <= MAX_MUSIC_VOLUME)
            g_music_volume = n;
        else
            return false;
    } else {
        g_music_volume = DEFAULT_MUSIC_VOLUME;
    }
    return true;
}

void write_music_volume_option() {
    if (g_music_volume != -1) {
        char str[3];
        assert(MAX_MUSIC_VOLUME <= 99);
        assert(g_music_volume >= 0 && g_music_volume <= MAX_MUSIC_VOLUME);
        if (sprintf(str, "%d", g_music_volume) > 0)
            al_set_config_value(g_config, SFX, MUSIC, str);
    }
}
