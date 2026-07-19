/*
 * Gameover-related routines
 */

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <assert.h>
#include <stdbool.h>

#include "fonts.h"
#include "gameover.h"
#include "gameplay.h"
#include "gfx.h"
#include "keyboard.h"
#include "scene.h"
#include "shared_state.h"
#include "sound.h"

int g_gameover_item;

/**
 * @brief Initialize the gameover scene
 */
void initialize_gameover() {
    g_gameover_item = 0;
}

/**
 * @brief Update the gameover scene one tick
 */
void update_gameover() {
    bool select = is_key_pressed(ALLEGRO_KEY_ENTER) || is_key_pressed(ALLEGRO_KEY_SPACE);
    bool escape = is_key_pressed(ALLEGRO_KEY_ESCAPE);
    bool up = is_key_pressed(ALLEGRO_KEY_UP);
    bool down = is_key_pressed(ALLEGRO_KEY_DOWN);
    if (down) {
        play_sound("menu_choose.ogg", false, false, NULL);
        g_gameover_item = (g_gameover_item + 1) % 4;
    } else if (up) {
        play_sound("menu_choose.ogg", false, false, NULL);
        g_gameover_item = (g_gameover_item + 3) % 4;
    } else if (escape && (g_gameover_item != 3)) {
        play_sound("menu_choose.ogg", false, false, NULL);
        g_gameover_item = 3;
    } else if ((escape || select) && (g_gameover_item == 3)) {
        play_sound("menu_change.ogg", false, false, NULL);
        play_sound("tryagain.ogg", false, false, NULL);
        g_scene = MENU_SCENE;
    } else if (select) {
        switch (g_gameover_item) {
        case 0:
            play_sound("menu_change.ogg", false, false, NULL);
            play_sound("tryagain.ogg", false, false, NULL);
            g_scene = GAMEPLAY_SCENE;
            break;
        case 1:
            // TODO watch replay
            break;
        case 2:
            // TODO save replay
            break;
        }
    }
}

static void draw_grid() {
    unsigned int x, y;
    for (x = 0; x < 640; x += 2) {
        al_draw_line(x, 0, x, 480, al_map_rgb(0, 0, 0), 0);
    }
    for (y = 0; y < 480; y += 2) {
        al_draw_line(0, y, 640, y, al_map_rgb(0, 0, 0), 0);
    }
}

static void draw_gameover_menu(const struct shared_state *shared_state) {
    ALLEGRO_BITMAP *menu_bullet = get_gfx_bitmap("menu_bullet.bmp");
    ALLEGRO_BITMAP *replay_bg = get_gfx_bitmap("replay_bg.bmp");
    int width = al_get_bitmap_width(replay_bg);
    int height = al_get_bitmap_height(replay_bg);
    int gameover_item = shared_state->gameover_item;
    al_draw_scaled_bitmap(replay_bg, 0, 0, width, height, 120, 140, 380, 170, 0);
    al_draw_text(g_font1, al_map_rgb(255, 255, 255),
        180, 160, ALLEGRO_ALIGN_LEFT,
        "Play Again");
    al_draw_text(g_font1, al_map_rgb(255, 255, 255),
        180, 188, ALLEGRO_ALIGN_LEFT,
        "Watch Replay");
    al_draw_text(g_font1, al_map_rgb(255, 255, 255),
        180, 216, ALLEGRO_ALIGN_LEFT,
        "Save Replay");
    al_draw_text(g_font1, al_map_rgb(255, 255, 255),
        180, 244, ALLEGRO_ALIGN_LEFT,
        "Main Menu");
    al_draw_bitmap(menu_bullet, 180 - al_get_bitmap_width(menu_bullet), 152 + 28 * gameover_item, 0);
}

/**
 * @brief Draw the gameover scene
 *
 * @param shared_state shared state
 */
void draw_gameover(const struct shared_state *shared_state) {
    draw_gameplay(shared_state);
    draw_grid();
    draw_gameover_menu(shared_state);
}
