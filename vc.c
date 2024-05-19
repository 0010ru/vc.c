#define VC_C_IMPLEMENTATION

// ------------------------------------

#ifndef VC_C
#define VC_C

#include <stdint.h>
#include <X11/Xlib.h>

typedef enum {
    VC_EVENT_UNKNOWN,
    VC_EVENT_EXPOSE,
    VC_EVENT_KEY_PRESS,
} VC_EventType;

typedef struct {
    const char *name;
    uint16_t width;
    uint16_t height;
} VC_WindowParams;

typedef struct {
    Display *x11_display;
} VC_Display;

typedef struct {
    VC_Display *display;
    Window x11_window;
    VC_WindowParams *params;
} VC_Window;

VC_Window vc_create_window(VC_WindowParams *params);
void vc_handle_event(VC_Window *window, VC_EventType event_type);
VC_EventType vc_get_next_event(VC_Window *window);
void vc_run_event_loop(VC_Window *window);
void vc_window_destroy(VC_Window *window);

#endif // VC_C

// ------------------------------------

#ifdef VC_C_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>


VC_Window vc_create_window(VC_WindowParams *params) {
    Display *x_display = XOpenDisplay(NULL);
    if (x_display == NULL) {
        fprintf(stderr, "ERROR: Cannot open X11 display!\n");
        exit(EXIT_FAILURE);
    }
    
    int screen = DefaultScreen(x_display);
    unsigned long black = BlackPixel(x_display, screen);
    unsigned long white = WhitePixel(x_display, screen);
    Window root_x_window = RootWindow(x_display, screen);
    Window x_window = XCreateSimpleWindow(
        x_display, // display
        root_x_window, // parent
        100, 100, // x, y
        params->width, params->height, 
        1, // border width
        black, white // border, background
    );

    if (!x_window) {
        fprintf(stderr, "ERROR: Failed to create X11 window!\n");
        XCloseDisplay(x_display);
        exit(EXIT_FAILURE);
    }

    XStoreName(x_display, x_window, params->name);
    XSelectInput(x_display, x_window, ExposureMask | KeyPressMask);
    XMapWindow(x_display, x_window);

    VC_Display* display = (VC_Display*)malloc(sizeof(VC_Display));
    display->x11_display = x_display;
    return (VC_Window) {display, x_window, params};
}

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
    XNextEvent(display->x11_display, &xevent);
    return vc_map_x11_event(xevent);
}

void vc_window_destroy_x11(VC_Window *window) {
    if (window->display != NULL) {
        if (window->x11_window != 0) {
            XDestroyWindow(window->display->x11_display, window->x11_window);
        }
        XCloseDisplay(window->display->x11_display);
    }
}

void vc_handle_event_expose(VC_Window *window) {
    XClearWindow(window->display->x11_display, window->x11_window);
}

void vc_handle_event(VC_Window *window, VC_EventType event_type) {
    switch (event_type) {
        case VC_EVENT_EXPOSE:
            vc_handle_event_expose(window);
            break;
        default:
            break;
    }
}

VC_EventType vc_get_next_event(VC_Window *window) {
    return vc_get_next_event_x11(window->display);
}

void vc_run_event_loop(VC_Window *window) {
    int running = 1;
    while (running) {
        VC_EventType event_type = vc_get_next_event(window); 
        if (event_type == VC_EVENT_UNKNOWN) break;
        vc_handle_event(window, event_type);
    }
}

void vc_window_destroy(VC_Window *window) {
    vc_window_destroy_x11(window);
    free(window->display);
}

#endif // VC_C_IMPLEMENTATION

// ------------------------------------

#if 1
int main() {
    VC_WindowParams params = {"My X11 Window", 800, 600};
    VC_Window window = vc_create_window(&params);
    vc_run_event_loop(&window);
    vc_window_destroy(&window);
    return 0;
}
#endif