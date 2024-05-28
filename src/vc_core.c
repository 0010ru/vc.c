#include <stdio.h>
#include <stdlib.h>

#include "../include/vc_core.h"
#include "../include/vc_arena.h"

// --- Native Display ---

NativeDisplay *vc_create_native_display() {
    NativeDisplay *native_display = XOpenDisplay(NULL);
    if (native_display == NULL) {
        fprintf(stderr, "ERROR: Cannot open X11 display!\n");
        return NULL;
    }
    return native_display;
}

// --- Native Window ---

void setup_native_window(NativeDisplay *native_display, NativeWindow *native_window, VC_WindowParams *params) {
    XStoreName(native_display, *native_window, params->name);
    XSelectInput(native_display, *native_window, ExposureMask | KeyPressMask);
    XMapWindow(native_display, *native_window);
}

NativeWindow *vc_create_native_window(NativeDisplay *native_display, VC_WindowParams *params, VC_Arena *arena) {
    NativeWindow *native_window = (NativeWindow *)vc_arena_alloc(arena, sizeof(NativeWindow));
    if (native_window == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for native window!\n");
        return NULL;
    }

    int screen = DefaultScreen(native_display);
    uint64_t black = BlackPixel(native_display, screen);
    uint64_t white = WhitePixel(native_display, screen);
    NativeWindow root_native_window = RootWindow(native_display, screen);
    *native_window = XCreateSimpleWindow(
        native_display, // display
        root_native_window, // parent
        100, 100, // x, y
        params->width, params->height, 
        1, // border width
        black, white // border, background
    );
    if (*native_window == 0) {
        fprintf(stderr, "ERROR: Failed to create X11 window!\n");
        XCloseDisplay(native_display);
        vc_m_free(arena, native_display);
        return NULL;
    }

    setup_native_window(native_display, native_window, params);

    return native_window;
}

// --- Window Creation ---

VC_Window vc_create_window_x11(VC_WindowParams *params, VC_Arena *arena) {
    NativeDisplay *native_display = vc_create_native_display();
    NativeWindow *native_window = vc_create_native_window(native_display, params, arena);

    VC_Display* display = (VC_Display*)malloc(sizeof(VC_Display)); 
    if (display == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for display!\n");
        XDestroyWindow(native_display, *native_window);
        XCloseDisplay(native_display);
        vc_m_free(arena, native_window);
        exit(EXIT_FAILURE);  // TODO: to retrurn
    }
    display->native_display = native_display;

    return (VC_Window) {display, *native_window, params};
}

VC_Window vc_create_window(VC_WindowParams *params, VC_Arena *arena) {
    return vc_create_window_x11(params, arena);
}

// --- Event Handling ---

VC_EventType vc_map_x11_event(XEvent event) {
    switch (event.type) {
        case Expose:
            return VC_EVENT_EXPOSE;
        case KeyPress:
            return VC_EVENT_KEY_PRESS;
        default:
            return VC_EVENT_UNKNOWN;
    }
}

VC_EventType vc_get_next_event_x11(VC_Display *display) {
    XEvent xevent;
    XNextEvent(display->native_display, &xevent);
    return vc_map_x11_event(xevent);
}

VC_EventType vc_get_next_event(VC_Window *window) {
    return vc_get_next_event_x11(window->display);
}

void vc_run_event_loop(VC_Window *window, VC_F_HandleEvent vc_handle_event) {
    int running = 1;
    while (running) {
        VC_EventType event_type = vc_get_next_event(window); 
        if (event_type == VC_EVENT_UNKNOWN) break;
        vc_handle_event(window, event_type);
    }
}

// --- Window Destruction ---

void vc_window_destroy_x11(VC_Window *window) {
    if (window->display != NULL) {
        if (window->native_window != 0) {
            XDestroyWindow(window->display->native_display, window->native_window);
        }
        XCloseDisplay(window->display->native_display);
    }
}

void vc_window_destroy(VC_Window *window) {
    vc_window_destroy_x11(window);
    free(window->display);
}