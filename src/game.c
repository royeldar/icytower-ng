/*
 * Main game loop
 */

#define ALLEGRO_UNSTABLE
#include <allegro5/allegro.h>
#include <stdbool.h>
#include <stdio.h>

#include "config.h"
#include "events.h"
#include "fullscreen.h"
#include "game.h"
#include "keyboard.h"
#include "music_volume.h"
#include "render.h"
#include "scene.h"
#include "sfx.h"
#include "shared_state.h"
#include "synchronization.h"

#define FPS 50

#define SAMPLES 16

#define SFX_DIR "sfx"

#define CONFIG_FILE "config.ini"

static struct shared_state initial_shared_state;

ALLEGRO_TIMER *g_timer = NULL;

static bool quit = false;

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

    // initialize fullscreen variable
    initial_shared_state.fullscreen = g_fullscreen;

    // initialize scene variable
    initial_shared_state.scene = g_scene;

    // initialize shared state used by the rendering thread
    initialize_shared_state(&initial_shared_state);

    // initialize game scene
    initialize_scene();

    return true;
}

void update_frame(struct shared_state *shared_state) {
    update_scene(&quit);
    shared_state->fullscreen = g_fullscreen;
    shared_state->scene = g_scene;
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
