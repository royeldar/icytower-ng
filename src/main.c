#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>

#include "events.h"
#include "flatpak.h"
#include "game.h"
#include "render.h"
#include "synchronization.h"

#define APP_NAME    "io.github.royeldar.icytower"

int main(int argc, char **argv) {
    ALLEGRO_THREAD *thread = NULL;

    // retrieve flatpak app id
    g_flatpak_id = getenv("FLATPAK_ID");

    // set app name
    al_set_app_name(APP_NAME);

    // initialize allegro system
    if (!al_init()) {
        printf("al_init() failed\n");
        goto cleanup;
    }

    // initialize the image addon
    if (!al_init_image_addon()) {
        printf("al_init_image_addon() failed\n");
        goto cleanup;
    }

    // install keyboard driver
    if (!al_install_keyboard()) {
        printf("al_install_keyboard() failed\n");
        goto cleanup;
    }

    // install audio subsystem
    if (!al_install_audio()) {
        printf("al_install_audio() failed\n");
        goto cleanup;
    }

    // initialize the audio codecs addon
    if (!al_init_acodec_addon()) {
        printf("al_init_acodec_addon() failed\n");
        goto cleanup;
    }

    // initialize the font addon
    if (!al_init_font_addon()) {
        printf("al_init_font_addon() failed\n");
        goto cleanup;
    }

    // setup stuff
    if (!game_setup()) {
        printf("game_setup() failed\n");
        goto cleanup;
    }

    // create a mutex object
    g_mutex = al_create_mutex();
    if (g_mutex == NULL) {
        printf("al_create_mutex() failed\n");
        goto cleanup;
    }

    // create a condition variable
    g_cond = al_create_cond();
    if (g_cond == NULL) {
        printf("al_create_cond() failed\n");
        goto cleanup;
    }

    // spawn a new thread
    thread = al_create_thread(render_main, NULL);
    if (thread == NULL) {
        printf("al_create_thread() failed\n");
        goto cleanup;
    }

    // start the new thread
    // note that this creates a display and registers its event source
    al_start_thread(thread);

    // wait for setup to finish
    if (wait_for_acknowledgement() != STATUS_SUCCESS) {
        printf("render_setup() failed\n");
        goto cleanup;
    }

    // register timer event source
    al_register_event_source(g_event_queue, al_get_timer_event_source(g_timer));

    // register keyboard event source
    al_register_event_source(g_event_queue, al_get_keyboard_event_source());

    // start timer
    al_start_timer(g_timer);

    // do main game loop
    game_loop();

    // stop timer
    al_stop_timer(g_timer);

    // unregister timer event source
    al_unregister_event_source(g_event_queue, al_get_timer_event_source(g_timer));

    // unregister keyboard event source
    al_unregister_event_source(g_event_queue, al_get_keyboard_event_source());

    // pause event queue
    al_pause_event_queue(g_event_queue, true);

    // flush event queue
    al_flush_event_queue(g_event_queue);

cleanup:
    if (thread != NULL) {
        // al_destroy_thread also calls al_join_thread
        // note that this destroys the display and unregisters its event source
        al_destroy_thread(thread);
    }
    if (g_mutex != NULL)
        al_destroy_mutex(g_mutex);
    if (g_cond != NULL)
        al_destroy_cond(g_cond);
    game_cleanup();
    return 0;
}
