/*
 * Main game loop
 */

#define ALLEGRO_UNSTABLE
#include <allegro5/allegro.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "character.h"
#include "config.h"
#include "controls.h"
#include "events.h"
#include "eye_candy.h"
#include "floor.h"
#include "fullscreen.h"
#include "game.h"
#include "gameplay.h"
#include "keyboard.h"
#include "level.h"
#include "menu.h"
#include "music_volume.h"
#include "random.h"
#include "render.h"
#include "scene.h"
#include "screenshake.h"
#include "sfx.h"
#include "shared_state.h"
#include "sound_volume.h"
#include "synchronization.h"

#define FPS 50

#define SAMPLES 16

#define SFX_DIR "sfx"

#define CONFIG_FILE "config.ini"

static struct shared_state initial_shared_state;

ALLEGRO_TIMER *g_timer = NULL;

static bool quit = false;

static unsigned int frame = 0;

bool game_setup() {
    // create an empty event queue
    g_event_queue = al_create_event_queue();
    if (g_event_queue == NULL) {
        printf("al_create_event_queue() failed\n");
        return false;
    }

    // create a stopped timer
    g_timer = al_create_timer(1.0 / FPS);
    if (g_timer == NULL) {
        printf("al_create_timer() failed\n");
        return false;
    }

    // reserve audio samples
    // note that this creates a default mixer and a default voice
    if (!al_reserve_samples(SAMPLES)) {
        printf("al_reserve_samples() failed\n");
        return false;
    }

    // load audio samples
    if (!load_sfx_samples(SFX_DIR)) {
        printf("load_sfx_samples() failed\n");
        return false;
    }

    // load configuration
    if (!load_config(CONFIG_FILE)) {
        printf("load_config() failed\n");
        return false;
    }

    // read fullscreen option from configuration
    if (!read_fullscreen_option()) {
        printf("read_fullscreen_option() failed\n");
        return false;
    }

    // read music volume option from configuration
    if (!read_music_volume_option()) {
        printf("read_music_volume_option() failed\n");
        return false;
    }

    // read sound volume option from configuration
    if (!read_sound_volume_option()) {
        printf("read_sound_volume_option() failed\n");
        return false;
    }

    // read eye candy option from configuration
    if (!read_eye_candy_option()) {
        printf("read_eye_candy_option() failed\n");
        return false;
    }

    // read controls options from configuration
    if (!read_controls_options()) {
        printf("read_controls_options() failed\n");
        return false;
    }

    // read character option from configuration
    if (!read_character_option()) {
        printf("read_character_option() failed\n");
        return false;
    }

    // read start floor option from configuration
    if (!read_start_floor_option()) {
        printf("read_start_floor_option() failed\n");
        return false;
    }

    // initialize fullscreen variable
    initial_shared_state.fullscreen = g_fullscreen;

    // initialize scene variable
    initial_shared_state.scene = (int)g_scene;

    // initialize menu variables
    initial_shared_state.menu_page = (int)g_menu_page;
    initial_shared_state.menu_item = g_menu_item;

    // initialize volume variables
    initial_shared_state.sound_volume = g_sound_volume;
    initial_shared_state.music_volume = g_music_volume;

    // initialize eye candy variable
    initial_shared_state.eye_candy = g_eye_candy;

    // initialize controls variables
    initial_shared_state.left_key = g_left_key;
    initial_shared_state.right_key = g_right_key;
    initial_shared_state.jump_key = g_jump_key;
    initial_shared_state.pause_key = g_pause_key;
    initial_shared_state.rejump = g_rejump;

    // initialize character variable
    initial_shared_state.character = g_character;

    // initialize start floor variable
    initial_shared_state.start_floor = g_start_floor;

    // initialize shared state used by the rendering thread
    initialize_shared_state(&initial_shared_state);

    // seed random number generators
    srand_msvc(time(NULL));
    rand_msvc();
    srand_custom(rand_msvc() % 2367);

    // initialize game scene
    initialize_scene();

    return true;
}

void update_frame(struct shared_state *shared_state) {
    update_scene(&quit);
    shared_state->frame = ++frame;
    shared_state->offset_x = g_offset_x;
    shared_state->offset_y = g_offset_y;
    shared_state->fullscreen = g_fullscreen;
    shared_state->scene = (int)g_scene;
    shared_state->menu_page = (int)g_menu_page;
    shared_state->menu_item = g_menu_item;
    shared_state->sound_volume = g_sound_volume;
    shared_state->music_volume = g_music_volume;
    shared_state->eye_candy = g_eye_candy;
    shared_state->left_key = g_left_key;
    shared_state->right_key = g_right_key;
    shared_state->jump_key = g_jump_key;
    shared_state->pause_key = g_pause_key;
    shared_state->rejump = g_rejump;
    shared_state->character = g_character;
    shared_state->start_floor = g_start_floor;
    shared_state->menu_animation_ticks = g_menu_animation_ticks;
    shared_state->heroface_animation_frame = g_heroface_animation_frame;
    memcpy(shared_state->platforms, g_platforms, sizeof(g_platforms));
    shared_state->screen_y = g_screen_y;
    shared_state->x = g_x;
    shared_state->y = g_y;
    shared_state->dx = g_dx;
    shared_state->dy = g_dy;
    shared_state->jump_state = (int)g_jump_state;
    shared_state->edge_state = (int)g_edge_state;
    shared_state->spinning = g_spinning;
    shared_state->rotation_angle = g_rotation_angle;
    shared_state->gameplay_animation_ticks = g_gameplay_animation_ticks;
    shared_state->character_animation_frame = g_character_animation_frame;
    shared_state->death = g_death;
    shared_state->pause = g_pause;
    shared_state->escape = g_escape;
    shared_state->clock_ticks = g_clock_ticks;
    shared_state->clock_speed = g_clock_speed;
    shared_state->hurryup_y = g_hurryup_y;
    shared_state->combo_timer = g_combo_timer;
    shared_state->combo_current = g_combo_current;
    shared_state->combo_score = g_combo_score;
    shared_state->combo_last = g_combo_last;
    shared_state->combo_best = g_combo_best;
    shared_state->reward_gfx = g_reward_gfx;
    shared_state->reward_timer = g_reward_timer;
    shared_state->reward_size = g_reward_size;
    shared_state->last_level = g_last_level;
}

void game_loop() {
    bool pause = false;
    while (!quit) {
        bool update = false;
        ALLEGRO_EVENT event;
        al_wait_for_event(g_event_queue, &event);
        switch (event.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            quit = true;
            break;
        case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
            send_event(HALT_DRAWING);
            wait_for_acknowledgement();
            break;
        case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
            send_event(RESUME_DRAWING);
            wait_for_acknowledgement();
            break;
        case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
            pause = true;
            al_stop_timer(g_timer);
            al_clear_keyboard_state(NULL);
            clear_keyboard_state();
            break;
        case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
            pause = false;
            al_resume_timer(g_timer);
            break;
        case ALLEGRO_EVENT_TIMER:
            if (!pause)
                update = true;
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
        case ALLEGRO_EVENT_KEY_UP:
            handle_keyboard_event(&event.keyboard);
            break;
        }
        if (update) {
            struct shared_state *shared_state = get_shared_state_for_writing();
            update_frame(shared_state);
            write_shared_state();
            do_keyboard_tick();
        }
    }
}

void game_cleanup() {
    if (g_config != NULL) {
        // write fullscreen option to configuration
        write_fullscreen_option();
        // write music volume option to configuration
        write_music_volume_option();
        // write sound volume option to configuration
        write_sound_volume_option();
        // write eye candy option to configuration
        write_eye_candy_option();
        // write controls options to configuration
        write_controls_options();
        // write character option to configuration
        write_character_option();
        // write start floor option to configuration
        write_start_floor_option();
        // save configuration
        save_config(CONFIG_FILE);
    }
    destroy_config();
    al_stop_samples();
    destroy_sfx_samples();
    if (g_timer != NULL)
        al_destroy_timer(g_timer);
    if (g_event_queue != NULL)
        al_destroy_event_queue(g_event_queue);
}
