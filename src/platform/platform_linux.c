#include <errno.h>

#include "platform.h"

#if defined(PLATFORM_LINUX)
#include "../core/asserts.h"
#include "../core/log.h"

#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

typedef struct Platform_Linux {
        xcb_connection_t* connection;
        xcb_screen_t* screen;
        xcb_window_t window_id;

        xcb_intern_atom_reply_t* window_protocols_reply;
        xcb_intern_atom_reply_t* delete_window_reply;
} Platform_Linux;

// Request the atom_t identifier for the given 
static xcb_intern_atom_cookie_t request_intern_atom(const Platform_Linux* state, const char* atom_name) {
        assertion(state->connection != NULL);
        return xcb_intern_atom(state->connection, 0, strlen(atom_name), atom_name);
}

static xcb_intern_atom_reply_t* recv_intern_atom(const Platform_Linux* state, const xcb_intern_atom_cookie_t request_cookie) {
        assertion(state->connection != NULL);
        return xcb_intern_atom_reply(state->connection, request_cookie, NULL);
}

bool platform_init(const Platform_Init_Info* init_info, Platform* platform) {
        *platform = (Platform){};
        platform->internal_state = malloc(sizeof(Platform_Linux));
        Platform_Linux* state = (Platform_Linux*)platform->internal_state; 

        i32 screen_index = 0;
        // Xcb Connection
        {
                state->connection = xcb_connect(NULL, &screen_index);
                if (state->connection == NULL) {
                        log_fatal("Failed to find xcb screen");
                        goto platform_init_failure;
                }
        }

        // Xcb Screen
        {
                const xcb_setup_t* setup = xcb_get_setup(state->connection);
                xcb_screen_iterator_t screen_it = xcb_setup_roots_iterator(setup);
                for (i32 i = 0; i < screen_index; ++i) xcb_screen_next(&screen_it);
                state->screen = screen_it.data;
                if (state->screen == NULL) {
                        log_fatal("Failed to find xcb screen");
                        goto platform_init_failure;
                }
        }

        // Xcb Window
        {
                const u32 event_mask =
                        XCB_EVENT_MASK_BUTTON_PRESS |
                        XCB_EVENT_MASK_BUTTON_RELEASE |
                        XCB_EVENT_MASK_KEY_PRESS |
                        XCB_EVENT_MASK_KEY_RELEASE |
                        XCB_EVENT_MASK_EXPOSURE |
                        XCB_EVENT_MASK_POINTER_MOTION |
                        XCB_EVENT_MASK_STRUCTURE_NOTIFY;

                const u32 value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
                const u32 values[2] = {state->screen->black_pixel, event_mask};

                state->window_id = xcb_generate_id(state->connection);
                xcb_create_window(
                                state->connection,
                                state->screen->root_depth,
                                state->window_id,
                                state->screen->root,
                                init_info->window_x,
                                init_info->window_y,
                                init_info->window_width,
                                init_info->window_height,
                                0, // window border
                                XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                state->screen->root_visual,
                                value_mask,
                                values);

                // Set window title
                xcb_change_property(
                                state->connection,
                                XCB_PROP_MODE_REPLACE,                          // mode
                                state->window_id,                               // window
                                XCB_ATOM_WM_NAME,                               // property (xcb_atom_t)
                                XCB_ATOM_STRING,                                // type (xcb_atom_t)
                                sizeof(char) * 8,                               // format (in bits)
                                strlen(init_info->window_title),                // data len (u32)
                                init_info->window_title                         // data (const void*)
                                );


                const xcb_intern_atom_cookie_t window_protocols_cookie = request_intern_atom(state, "WM_PROTOCOLS");
                const xcb_intern_atom_cookie_t delete_window_cookie = request_intern_atom(state, "WM_DELETE_WINDOW");
                state->window_protocols_reply = recv_intern_atom(state, window_protocols_cookie);
                state->delete_window_reply = recv_intern_atom(state, delete_window_cookie);
                if (state->window_protocols_reply == NULL || state->delete_window_reply == NULL) {
                        log_fatal("xcb returned an invalid reply");
                        goto platform_init_failure;
                }

                xcb_change_property(
                                state->connection,
                                XCB_PROP_MODE_REPLACE,                  // mode
                                state->window_id,                       // window
                                state->window_protocols_reply->atom,    // property (xcb_atom_t)
                                XCB_ATOM_ATOM,                          // type (xcb_atom_t)
                                sizeof(xcb_atom_t) * 8,                 // format (u8, in number of bits)
                                1,                                      // data len (u32)
                                &state->delete_window_reply->atom       // data (const void*)
                                );

                xcb_map_window(state->connection, state->window_id);
        }

        const bool flush_sucessful = xcb_flush(state->connection) > 0;
        if (!flush_sucessful) {
                log_fatal("Xcb flush failed");
                goto platform_init_failure;
        }

        return true;

platform_init_failure:
        platform_deinit(platform);
        return false;
}

void platform_deinit(Platform* platform) {
        if (platform == NULL) {
                return;
        }

        Platform_Linux* state = (Platform_Linux*)platform->internal_state;
        assertion(state != NULL);

        if (state->delete_window_reply)         free(state->delete_window_reply);
        if (state->window_protocols_reply)      free(state->window_protocols_reply);
        if (state->window_id)                   xcb_destroy_window(state->connection, state->window_id);
        if (state->connection)                  xcb_disconnect(state->connection);

        free(state); 

        *platform = (Platform){};
}

bool platform_poll_events(Platform* platform, Platform_Events* platform_events) {
        *platform_events = (Platform_Events){};
        const Platform_Linux* state = (Platform_Linux*)platform->internal_state;

        xcb_generic_event_t* event;
        while ((event = xcb_poll_for_event(state->connection))) {
                switch (event->response_type & ~(i32)0x80) {
                        case XCB_KEY_PRESS:
                        case XCB_KEY_RELEASE:
                        {
                                // Key presses and releases
                        } break;
                        case XCB_BUTTON_PRESS:
                        case XCB_BUTTON_RELEASE:
                        {
                                // Mouse button presses and releases
                        } break;
                        case XCB_MOTION_NOTIFY:
                        {
                                // Window resizing
                        } break;
                        case XCB_CLIENT_MESSAGE:
                        {
                                const xcb_client_message_event_t* client_message = (xcb_client_message_event_t*)event;
                                if (client_message->data.data32[0] == state->delete_window_reply->atom) {
                                        platform_events->window_should_close = true;
                                }
                        } break;
                        default: ;
                }

                free(event);
        }

        return !platform_events->window_should_close; // return false if window should close
}

void platform_console_write(const char* message, const u8 log_level) {
        const char* colors[LOG_LEVEL_COUNT] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
        fprintf(stdout, "\033[%sm%s\033[0m", colors[log_level], message);
}

void platform_console_write_error(const char* message, const u8 log_level) {
        const char* colors[LOG_LEVEL_COUNT] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
        fprintf(stderr, "\033[%sm%s\033[0m", colors[log_level], message);
}

f64 platform_get_absolute_time() {
        struct timespec now;
        if (clock_gettime(CLOCK_MONOTONIC, &now) == -1) {
                log_fatal("clock_gettime failed. errno: %s", strerror(errno));
                asserts_debug_break();
                exit(-1);
        }
        const f64 seconds_per_nanosecond = 0.000000001;
        const f64 absolute_time_in_seconds = now.tv_sec + (now.tv_nsec * seconds_per_nanosecond);
        return absolute_time_in_seconds;
}

void platform_thread_sleep(const u64 milliseconds) {
#if _POSIX_C_SOURCE >= 199309L
        struct timespec requested_sleep_time;
        requested_sleep_time.tv_sec = (__time_t)(milliseconds / 1000);                        // in seconds
        requested_sleep_time.tv_nsec = (__time_t)((milliseconds % 1000) * 1000 * 1000);       // rest in nanoseconds
        if (nanosleep(&requested_sleep_time, NULL) == -1) {
                log_fatal("nanosleep failed. errno: %s", strerror(errno));
                asserts_debug_break();
                exit(-1);
        }
#else
#error "TODO implement nanosleep alternative for distributions that don't support it"
#endif
}

void* platform_memory_allocate(const u64 size) {
        return malloc(size);
}

void platform_memory_free(const void* ptr) {
        free((void*)ptr);
}

void platform_memory_zero(void* dest, const u64 size) {
        memset(dest, 0, size);
}

void platform_memory_set(void* dest, const int value, const u64 size) {
        memset(dest, value, size);
}

void platform_memory_copy(void* dest, const void* src, u64 size) {
        memcpy(dest, src, size);
}

#endif // PLATFORM_LINUX

