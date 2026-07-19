/*
 * Rendering thread
 */

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "background.h"
#include "events.h"
#include "fonts.h"
#include "fullscreen.h"
#include "gfx.h"
#include "render.h"
#include "scene.h"
#include "shared_state.h"
#include "synchronization.h"

#define MASK_COLOR  (al_map_rgb(154, 20, 146))

#define WIDTH   640
#define HEIGHT  480

#define TITLE   "Icy Tower"

#define GFX_DIR "gfx"

static ALLEGRO_BITMAP *bitmap = NULL;
static ALLEGRO_DISPLAY *display = NULL;
static unsigned int frame = 0;
static bool fullscreen = false;

static bool convert_mask_to_alpha_callback(ALLEGRO_BITMAP *bitmap) {
    al_convert_mask_to_alpha(bitmap, MASK_COLOR);
    return true;
}

static int render_setup() {
    // create a 640x480 bitmap
    bitmap = al_create_bitmap(WIDTH, HEIGHT);
    if (bitmap == NULL) {
        printf("al_create_bitmap() failed\n");
        return STATUS_FAILURE;
    }
    // create a 640x480 display
    fullscreen = g_fullscreen;
    al_set_new_display_flags(ALLEGRO_OPENGL | (fullscreen ? ALLEGRO_FULLSCREEN_WINDOW : ALLEGRO_WINDOWED));
    display = al_create_display(WIDTH, HEIGHT);
    if (display == NULL) {
        printf("al_create_display() failed\n");
        return STATUS_FAILURE;
    }
    // register display event source
    al_register_event_source(g_event_queue, al_get_display_event_source(display));
    // set window title
    al_set_window_title(display, TITLE);
    // load bitmaps
    if (!load_gfx_bitmaps(GFX_DIR)) {
        printf("load_gfx_bitmaps() failed\n");
        return STATUS_FAILURE;
    }
    // convert mask color to alpha
    if (!iterate_gfx_bitmaps(convert_mask_to_alpha_callback)) {
        printf("iterate_gfx_bitmaps() failed\n");
        return STATUS_FAILURE;
    }
    // load fonts
    if (!create_fonts()) {
        printf("create_fonts() failed\n");
        return STATUS_FAILURE;
    }
    // create background texture
    if (!create_background_bitmap()) {
        printf("create_background_bitmap() failed\n");
        return STATUS_FAILURE;
    }
    return STATUS_SUCCESS;
}

static void render_frame(const struct shared_state *shared_state) {
    float width = al_get_display_width(display);
    float height = al_get_display_height(display);
    float scale = fminf(width / WIDTH, height / HEIGHT);
    int offset_x = shared_state->offset_x;
    int offset_y = shared_state->offset_y;
    al_set_target_bitmap(bitmap);
    draw_scene(shared_state);
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_scaled_bitmap(bitmap,
        offset_x, offset_y,
        WIDTH, HEIGHT,
        (width - WIDTH * scale) / 2, (height - HEIGHT * scale) / 2,
        WIDTH * scale, HEIGHT * scale,
        0);
    al_flip_display();
}

static void render_loop(ALLEGRO_THREAD *thread) {
    bool dont_draw = false;
    bool redraw = true;
    while (!al_get_thread_should_stop(thread)) {
        int event = receive_event();
        if (event == 0 && !dont_draw) {
            const struct shared_state *shared_state;
            read_shared_state();
            shared_state = get_shared_state_for_reading();
            if (shared_state->fullscreen != fullscreen) {
                fullscreen = shared_state->fullscreen;
                al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, fullscreen);
                redraw = true;
            }
            if (shared_state->frame > frame) {
                frame = shared_state->frame;
                redraw = true;
            }
            if (redraw) {
                render_frame(shared_state);
                redraw = false;
            }
        } else {
            switch (event) {
            case HALT_DRAWING:
                dont_draw = true;
                al_acknowledge_drawing_halt(display);
                break;
            case RESUME_DRAWING:
                al_acknowledge_drawing_resume(display);
                dont_draw = false;
                redraw = true;
                break;
            }
            acknowledge_event(STATUS_SUCCESS);
        }
    }
}

static void render_cleanup() {
    // destroy background texture
    destroy_background_bitmap();
    // destroy fonts
    destroy_fonts();
    // destroy bitmaps
    destroy_gfx_bitmaps();
    if (display != NULL) {
        // unregister display event source
        al_unregister_event_source(g_event_queue, al_get_display_event_source(display));
        // destroy display
        al_destroy_display(display);
    }
    if (bitmap != NULL) {
        // destroy bitmap
        al_destroy_bitmap(bitmap);
    }
}

void *render_main(ALLEGRO_THREAD *thread, void *arg) {
    int ret;

    // setup stuff
    ret = render_setup();

    // tell the main thread that setup has finished
    acknowledge_event(ret);

    // exit on failure
    if (ret != STATUS_SUCCESS)
        goto cleanup;

    // render loop
    render_loop(thread);

cleanup:
    // cleanup stuff
    render_cleanup();
    return NULL;
}
