/*
 * Menu-related routines
 */

#include <allegro5/allegro.h>
#include <assert.h>
#include <stdbool.h>

#include "character.h"
#include "controls.h"
#include "eye_candy.h"
#include "floor.h"
#include "fonts.h"
#include "fullscreen.h"
#include "gfx.h"
#include "keyboard.h"
#include "keymap.h"
#include "menu.h"
#include "music.h"
#include "music_volume.h"
#include "random.h"
#include "rebind.h"
#include "scene.h"
#include "shared_state.h"
#include "sound.h"
#include "sound_volume.h"

enum menu_page g_menu_page = MAIN_PAGE;
int g_menu_item = 0;

unsigned int g_menu_animation_ticks = 0;
int g_heroface_animation_frame = 0;

/**
 * @brief Initialize the menu scene
 */
void initialize_menu() {
    play_music("bg_menu.ogg");
}

static void update_heroface_animation() {
    g_menu_animation_ticks++;
    if (rand_custom() % 198 == 1) {
        if (++g_heroface_animation_frame == 3)
            g_heroface_animation_frame = 0;
    }
}

static void update_menu_page() {
    bool select = is_key_pressed(ALLEGRO_KEY_ENTER) || is_key_pressed(ALLEGRO_KEY_SPACE);
    bool escape = is_key_pressed(ALLEGRO_KEY_ESCAPE);
    bool up = is_key_pressed(ALLEGRO_KEY_UP);
    bool down = is_key_pressed(ALLEGRO_KEY_DOWN);
    bool left = is_key_pressed(ALLEGRO_KEY_LEFT);
    bool right = is_key_pressed(ALLEGRO_KEY_RIGHT);
    int count = 0;
    switch (g_menu_page) {
    case MAIN_PAGE:
        count = 5;
        break;
    case OPTIONS_PAGE:
        count = 5;
        break;
    case GAME_OPTIONS_PAGE:
        count = 3;
        break;
    case GFX_OPTIONS_PAGE:
        count = 3;
        break;
    case SOUND_OPTIONS_PAGE:
        count = 3;
        break;
    case CONTROLS_PAGE:
        count = 6;
        break;
    }
    assert(count > 0);
    if (down) {
        play_sound("menu_choose.ogg");
        g_menu_item = (g_menu_item + 1) % count;
    } else if (up) {
        play_sound("menu_choose.ogg");
        g_menu_item = (g_menu_item - 1 + count) % count;
    } else if (escape && (g_menu_item != count - 1)) {
        play_sound("menu_choose.ogg");
        g_menu_item = count - 1;
    } else if ((escape || select) && (g_menu_item == count - 1)) {
        play_sound("menu_change.ogg");
        switch (g_menu_page) {
        case MAIN_PAGE:
            g_scene = EXIT_SCENE;
            break;
        case OPTIONS_PAGE:
            g_menu_page = MAIN_PAGE;
            g_menu_item = 3;
            break;
        case GAME_OPTIONS_PAGE:
            g_menu_page = OPTIONS_PAGE;
            g_menu_item = 0;
            break;
        case GFX_OPTIONS_PAGE:
            g_menu_page = OPTIONS_PAGE;
            g_menu_item = 1;
            break;
        case SOUND_OPTIONS_PAGE:
            g_menu_page = OPTIONS_PAGE;
            g_menu_item = 2;
            break;
        case CONTROLS_PAGE:
            g_menu_page = OPTIONS_PAGE;
            g_menu_item = 3;
            break;
        }
    } else if (select) {
        if ((g_menu_page == MAIN_PAGE) && (g_menu_item == 0)) {
            play_sound("menu_change.ogg");
            stop_music();
            g_scene = GAMEPLAY_SCENE;
        } else if ((g_menu_page == MAIN_PAGE) && (g_menu_item == 1)) {
            // TODO replay
        } else if ((g_menu_page == MAIN_PAGE) && (g_menu_item == 2)) {
            play_sound("menu_change.ogg");
            g_scene = INSTRUCTIONS_SCENE;
        } else if ((g_menu_page == MAIN_PAGE) && (g_menu_item == 3)) {
            play_sound("menu_change.ogg");
            g_menu_page = OPTIONS_PAGE;
            g_menu_item = 0;
        } else if ((g_menu_page == OPTIONS_PAGE) && (g_menu_item == 0)) {
            play_sound("menu_change.ogg");
            g_menu_page = GAME_OPTIONS_PAGE;
            g_menu_item = 0;
        } else if ((g_menu_page == OPTIONS_PAGE) && (g_menu_item == 1)) {
            play_sound("menu_change.ogg");
            g_menu_page = GFX_OPTIONS_PAGE;
            g_menu_item = 0;
        } else if ((g_menu_page == OPTIONS_PAGE) && (g_menu_item == 2)) {
            play_sound("menu_change.ogg");
            g_menu_page = SOUND_OPTIONS_PAGE;
            g_menu_item = 0;
        } else if ((g_menu_page == OPTIONS_PAGE) && (g_menu_item == 3)) {
            play_sound("menu_change.ogg");
            g_menu_page = CONTROLS_PAGE;
            g_menu_item = 0;
        } else if ((g_menu_page == GFX_OPTIONS_PAGE) && (g_menu_item == 1)) {
            play_sound("menu_change.ogg");
            g_fullscreen = !g_fullscreen;
        } else if ((g_menu_page == CONTROLS_PAGE) && (g_menu_item < 4)) {
            play_sound("menu_change.ogg");
            g_scene = REBIND_SCENE;
        } else if ((g_menu_page == CONTROLS_PAGE) && (g_menu_item == 4)) {
            play_sound("menu_change.ogg");
            g_rejump = !g_rejump;
        }
    } else if (left || right) {
        if ((g_menu_page == GAME_OPTIONS_PAGE) && (g_menu_item == 0)) {
            play_sound("menu_change.ogg");
            if (left && (g_character > 0))
                g_character--;
            else if (right && (g_character < NUM_CHARACTERS - 1))
                g_character++;
        } else if ((g_menu_page == GAME_OPTIONS_PAGE) && (g_menu_item == 1)) {
            play_sound("menu_change.ogg");
            if (left && (g_start_floor > 0))
                g_start_floor--;
            else if (right && (g_start_floor < NUM_FLOORS - 1))
                g_start_floor++;
        } else if ((g_menu_page == GFX_OPTIONS_PAGE) && (g_menu_item == 0)) {
            play_sound("menu_change.ogg");
            if (left && (g_eye_candy > 0))
                g_eye_candy--;
            else if (right && (g_eye_candy < MAX_EYE_CANDY))
                g_eye_candy++;
        } else if ((g_menu_page == SOUND_OPTIONS_PAGE) && (g_menu_item == 0)) {
            play_sound("menu_change.ogg");
            if (left && (g_sound_volume > 0))
                g_sound_volume--;
            else if (right && (g_sound_volume < MAX_SOUND_VOLUME))
                g_sound_volume++;
        } else if ((g_menu_page == SOUND_OPTIONS_PAGE) && (g_menu_item == 1)) {
            play_sound("menu_change.ogg");
            if (left && (g_music_volume > 0)) {
                g_music_volume--;
                update_music();
            } else if (right && (g_music_volume < MAX_MUSIC_VOLUME)) {
                g_music_volume++;
                update_music();
            }
        }
    }
}

/**
 * @brief Update the menu scene one tick
 */
void update_menu() {
    update_heroface_animation();
    update_menu_page();
}

static void draw_heroface_animation(const struct shared_state *shared_state) {
    unsigned int menu_animation_ticks = shared_state->menu_animation_ticks;
    int heroface_animation_frame = shared_state->heroface_animation_frame;
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
    dy = (h / 2) + 10 + al_fixtoi(10 * al_fixsin(al_itofix(5 * menu_animation_ticks)));
    angle = al_fixtof(al_fixmul(
        5 * al_fixsin(al_itofix(3 * menu_animation_ticks) + (al_fixed)10),
        al_fixtorad_r));
    al_draw_rotated_bitmap(heroface, w / 2, h / 2, dx, dy, angle, 0);
}

static void draw_menu_page(const struct shared_state *shared_state) {
    ALLEGRO_BITMAP *menu_bullet = get_gfx_bitmap("menu_bullet.bmp");
    int character = shared_state->character;
    ALLEGRO_BITMAP *character_idle = get_gfx_bitmap(g_characters[character].gfx_idle1);
    int start_floor = shared_state->start_floor;
    ALLEGRO_BITMAP *floor_left = get_gfx_bitmap(g_floors[start_floor].left);
    ALLEGRO_BITMAP *floor_mid = get_gfx_bitmap(g_floors[start_floor].mid);
    ALLEGRO_BITMAP *floor_right = get_gfx_bitmap(g_floors[start_floor].right);
    bool fullscreen = shared_state->fullscreen;
    int sound_volume = shared_state->sound_volume;
    int music_volume = shared_state->music_volume;
    char sound_volume_str[MAX_SOUND_VOLUME + 1];
    char music_volume_str[MAX_MUSIC_VOLUME + 1];
    unsigned int i;
    int eye_candy = shared_state->eye_candy;
    char *eye_candy_str = NULL;
    int left_key = shared_state->left_key;
    int right_key = shared_state->right_key;
    int jump_key = shared_state->jump_key;
    int pause_key = shared_state->pause_key;
    bool rejump = shared_state->rejump;
    enum menu_page menu_page = (enum menu_page)shared_state->menu_page;
    int menu_item = shared_state->menu_item;
    switch (menu_page) {
    case MAIN_PAGE:
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 270, ALLEGRO_ALIGN_LEFT,
            "Start Game");
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 298, ALLEGRO_ALIGN_LEFT,
            "Load Replay");
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 326, ALLEGRO_ALIGN_LEFT,
            "Instructions");
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 354, ALLEGRO_ALIGN_LEFT,
            "Options");
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 382, ALLEGRO_ALIGN_LEFT,
            "Exit");
        break;
    case OPTIONS_PAGE:
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 270, ALLEGRO_ALIGN_LEFT,
            "Game Options");
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 298, ALLEGRO_ALIGN_LEFT,
            "GFX Options");
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 326, ALLEGRO_ALIGN_LEFT,
            "Sound Options");
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 354, ALLEGRO_ALIGN_LEFT,
            "Controls");
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 382, ALLEGRO_ALIGN_LEFT,
            "Back");
        break;
    case GAME_OPTIONS_PAGE:
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 270, ALLEGRO_ALIGN_LEFT,
            "Character:");
        al_draw_bitmap(character_idle, 330, 308 - al_get_bitmap_height(character_idle), 0);
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 298, ALLEGRO_ALIGN_LEFT,
            "Start Floor:");
        al_draw_bitmap(floor_left, 315, 308, 0);
        al_draw_bitmap(floor_mid, 336, 308, 0);
        al_draw_bitmap(floor_right, 352, 308, 0);
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 326, ALLEGRO_ALIGN_LEFT,
            "Back");
        break;
    case GFX_OPTIONS_PAGE:
        switch (eye_candy) {
        case 0:
            eye_candy_str = "None";
            break;
        case 1:
            eye_candy_str = "Some";
            break;
        case 2:
            eye_candy_str = "Lots";
            break;
        }
        assert(eye_candy_str != NULL);
        al_draw_textf(g_font1, al_map_rgb(255, 255, 255),
            40, 270, ALLEGRO_ALIGN_LEFT,
            "Eye Candy: %s", eye_candy_str);
        al_draw_textf(g_font1, al_map_rgb(255, 255, 255),
            40, 298, ALLEGRO_ALIGN_LEFT,
            "Fullscreen: %s", (fullscreen ? "Yes" : "No"));
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 326, ALLEGRO_ALIGN_LEFT,
            "Back");
        break;
    case SOUND_OPTIONS_PAGE:
        for (i = 0; i < MAX_SOUND_VOLUME; ++i) {
            sound_volume_str[i] = (i < sound_volume ? '}' : '{');
        }
        sound_volume_str[MAX_SOUND_VOLUME] = '\0';
        al_draw_textf(g_font1, al_map_rgb(255, 255, 255),
            40, 270, ALLEGRO_ALIGN_LEFT,
            "Sound:%s", sound_volume_str);
        for (i = 0; i < MAX_MUSIC_VOLUME; ++i) {
            music_volume_str[i] = (i < music_volume ? '}' : '{');
        }
        music_volume_str[MAX_MUSIC_VOLUME] = '\0';
        al_draw_textf(g_font1, al_map_rgb(255, 255, 255),
            40, 298, ALLEGRO_ALIGN_LEFT,
            "Music:%s", music_volume_str);
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 326, ALLEGRO_ALIGN_LEFT,
            "Back");
        break;
    case CONTROLS_PAGE:
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 270, ALLEGRO_ALIGN_LEFT,
            "Left:");
        al_draw_textf(g_font1, al_map_rgb(255, 255, 255),
            180, 270, ALLEGRO_ALIGN_LEFT,
            "(%s)", get_key_name(left_key));
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 298, ALLEGRO_ALIGN_LEFT,
            "Right:");
        al_draw_textf(g_font1, al_map_rgb(255, 255, 255),
            180, 298, ALLEGRO_ALIGN_LEFT,
            "(%s)", get_key_name(right_key));
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 326, ALLEGRO_ALIGN_LEFT,
            "Jump:");
        al_draw_textf(g_font1, al_map_rgb(255, 255, 255),
            180, 326, ALLEGRO_ALIGN_LEFT,
            "(%s)", get_key_name(jump_key));
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 354, ALLEGRO_ALIGN_LEFT,
            "Pause:");
        al_draw_textf(g_font1, al_map_rgb(255, 255, 255),
            180, 354, ALLEGRO_ALIGN_LEFT,
            "(%s)", get_key_name(pause_key));
        al_draw_textf(g_font1, al_map_rgb(255, 255, 255),
            40, 382, ALLEGRO_ALIGN_LEFT,
            "Rejump: %s", (rejump ? "Yes" : "No"));
        al_draw_text(g_font1, al_map_rgb(255, 255, 255),
            40, 410, ALLEGRO_ALIGN_LEFT,
            "Back");
        break;
    }
    al_draw_bitmap(menu_bullet, 40 - al_get_bitmap_width(menu_bullet), 262 + 28 * menu_item, 0);
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
    draw_heroface_animation(shared_state);
    draw_menu_page(shared_state);
}
