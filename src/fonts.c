/*
 * Font loading routines
 */

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <stdbool.h>

#include "fonts.h"
#include "gfx.h"

ALLEGRO_FONT *g_font1 = NULL;
ALLEGRO_FONT *g_font2 = NULL;
ALLEGRO_FONT *g_font3 = NULL;

/**
 * @brief Create fonts from bitmaps
 *
 * @return true if all fonts were created successfully
 * @return false otherwise
 */
bool create_fonts() {
    int ranges[] = { 0x20, 0x7f };
    g_font1 = al_grab_font_from_bitmap(get_gfx_bitmap("font1.bmp"), 1, ranges);
    g_font2 = al_grab_font_from_bitmap(get_gfx_bitmap("font2.bmp"), 1, ranges);
    g_font3 = al_grab_font_from_bitmap(get_gfx_bitmap("font3.bmp"), 1, ranges);
    return (g_font1 != NULL && g_font2 != NULL && g_font3 != NULL);
}

/**
 * @brief Destroy fonts
 */
void destroy_fonts() {
    if (g_font1 != NULL)
        al_destroy_font(g_font1);
    if (g_font2 != NULL)
        al_destroy_font(g_font2);
    if (g_font3 != NULL)
        al_destroy_font(g_font3);
}
