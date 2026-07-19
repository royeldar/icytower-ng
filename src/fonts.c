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

static void fix_font3_bitmap(ALLEGRO_BITMAP *bitmap) {
    int w = al_get_bitmap_width(bitmap);
    int h = al_get_bitmap_height(bitmap);
    int x, y;
    unsigned char bg_r, bg_g, bg_b;
    unsigned char r, g, b, a;
    int flags = al_get_new_bitmap_flags();
    al_set_target_bitmap(bitmap);
    al_set_new_bitmap_flags(flags | ALLEGRO_MEMORY_BITMAP);
    al_convert_bitmap(bitmap);
    al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
    for (x = 0; x < w; ++x) {
        for (y = 0; y < h; ++y) {
            ALLEGRO_COLOR c = al_get_pixel(bitmap, x, y);
            if (x == 0 && y == 0) {
                al_unmap_rgb(c, &bg_r, &bg_g, &bg_b);
            } else {
                al_unmap_rgba(c, &r, &g, &b, &a);
                if (a != 0 && !(r == bg_r && g == bg_g && b == bg_b)) {
                    c = al_map_rgb(255, 255, 255);
                    al_put_pixel(x, y, c);
                }
            }
        }
    }
    al_unlock_bitmap(bitmap);
    al_set_new_bitmap_flags(flags);
    al_convert_bitmap(bitmap);
}

/**
 * @brief Create fonts from bitmaps
 *
 * @return true if all fonts were created successfully
 * @return false otherwise
 */
bool create_fonts() {
    int ranges[] = { 0x20, 0x7f };
    ALLEGRO_BITMAP *font1_bitmap = get_gfx_bitmap("font1.bmp");
    ALLEGRO_BITMAP *font2_bitmap = get_gfx_bitmap("font2.bmp");
    ALLEGRO_BITMAP *font3_bitmap = get_gfx_bitmap("font3.bmp");
    fix_font3_bitmap(font3_bitmap);
    g_font1 = al_grab_font_from_bitmap(font1_bitmap, 1, ranges);
    g_font2 = al_grab_font_from_bitmap(font2_bitmap, 1, ranges);
    g_font3 = al_grab_font_from_bitmap(font3_bitmap, 1, ranges);
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
