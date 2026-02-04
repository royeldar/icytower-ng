/*
 * Menu-related routines
 */

#include <allegro5/allegro.h>
#include <stdbool.h>

#include "gfx.h"
#include "keyboard.h"
#include "menu.h"
#include "music.h"
#include "random.h"
#include "scene.h"
#include "shared_state.h"
#include "sound.h"

static unsigned int heroface_animation_ticks = 0;
static int heroface_animation_frame = 0;

/**
 * @brief Initialize the menu scene
 */
void initialize_menu() {
    play_music("bg_menu.ogg");
}

static void update_heroface_animation() {
    heroface_animation_ticks++;
    if (rand_custom() % 198 == 1) {
        if (++heroface_animation_frame == 3)
            heroface_animation_frame = 0;
    }
}

/**
 * @brief Update the menu scene one tick
 */
void update_menu() {
    update_heroface_animation();
    if (is_key_pressed(ALLEGRO_KEY_ESCAPE)) {
        play_sound("menu_change.ogg");
        g_scene = EXIT_SCENE;
    }
}

static void draw_heroface_animation() {
    ALLEGRO_BITMAP *heroface = NULL;
    float w, h, dx, dy, angle;
    switch (heroface_animation_frame) {
    case 0:
        heroface = get_gfx_bitmap("heroface000.bmp");
        break;
    case 1:
        heroface = get_gfx_bitmap("heroface001.bmp");
        break;
    case 2:
        heroface = get_gfx_bitmap("heroface002.bmp");
        break;
    }
    assert(heroface != NULL);
    w = al_get_bitmap_width(heroface);
    h = al_get_bitmap_height(heroface);
    dx = (w / 2) + 40;
    dy = (h / 2) + 10 + al_fixtoi(10 * al_fixsin(al_itofix(5 * heroface_animation_ticks)));
    angle = al_fixtof(al_fixmul(
        5 * al_fixsin(al_itofix(3 * heroface_animation_ticks) + (al_fixed)10),
        al_fixtorad_r));
    al_draw_rotated_bitmap(heroface, w / 2, h / 2, dx, dy, angle, 0);
}

/**
 * @brief Draw the menu scene
 *
 * @param shared_state shared state
 */
void draw_menu(const struct shared_state *shared_state) {
    ALLEGRO_BITMAP *title_bg = get_gfx_bitmap("title_bg.bmp");
    ALLEGRO_BITMAP *title = get_gfx_bitmap("title.bmp");
    al_draw_bitmap(title_bg, 0, 0, 0);
    al_draw_bitmap(title, 250, 20, 0);
    draw_heroface_animation();
}
