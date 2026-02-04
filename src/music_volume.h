#include <stdbool.h>

#define DEFAULT_MUSIC_VOLUME    6
#define MAX_MUSIC_VOLUME    10

extern int g_music_volume;

bool read_music_volume_option();
void write_music_volume_option();
