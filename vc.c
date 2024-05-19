#define VC_C_IMPLEMENTATION

// ------------------------------------
// ------------------------------------
// ------------------------------------
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

typedef Display NativeDisplay;
typedef Window NativeWindow;

typedef struct {
    NativeDisplay *native_display;
} VC_Display;

typedef struct {
    VC_Display *display;
    NativeWindow native_window;
    VC_WindowParams *params;
} VC_Window;

typedef void (*VC_F_HandleEvent)(VC_Window *window, VC_EventType event_type);

VC_Window vc_create_window(VC_WindowParams *params);
VC_EventType vc_get_next_event(VC_Window *window);
void vc_run_event_loop(VC_Window *window, VC_F_HandleEvent vc_handle_event);
void vc_window_destroy(VC_Window *window);

#endif // VC_C

// ------------------------------------
// ------------------------------------
// ------------------------------------
// ------------------------------------

#ifdef VC_C_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>

NativeDisplay *vc_create_native_display(void) {
    NativeDisplay *native_display = XOpenDisplay(NULL);
    if (native_display == NULL) {
        fprintf(stderr, "ERROR: Cannot open X11 display!\n");
        exit(EXIT_FAILURE);
    }
    return native_display;
}

void setup_native_window(NativeDisplay *native_display, NativeWindow *native_window , VC_WindowParams *params) {
    XStoreName(native_display, *native_window , params->name);
    XSelectInput(native_display, *native_window , ExposureMask | KeyPressMask);
    XMapWindow(native_display, *native_window );
}

Window *vc_create_native_window(NativeDisplay *native_display, VC_WindowParams *params) {
    NativeWindow *native_window  = (Window*)malloc(sizeof(Window)); // TODO: memory managment
    
    int screen = DefaultScreen(native_display);
    uint64_t black = BlackPixel(native_display, screen);
    uint64_t white = WhitePixel(native_display, screen);
    NativeWindow root_native_window  = RootWindow(native_display, screen);
    *native_window  = XCreateSimpleWindow(
        native_display, // display
        root_native_window , // parent
        100, 100, // x, y
        params->width, params->height, 
        1, // border width
        black, white // border, background
    );
    if (native_window  == NULL) {
        fprintf(stderr, "ERROR: Failed to create X11 window!\n");
        XCloseDisplay(native_display);
        exit(EXIT_FAILURE);
    }

    setup_native_window(native_display, native_window , params);

    return native_window ;
}

VC_Window vc_create_window_x11(VC_WindowParams *params) {
    NativeDisplay *native_display = vc_create_native_display();
    NativeWindow *native_window  = vc_create_native_window(native_display, params);

    VC_Display* display = (VC_Display*)malloc(sizeof(VC_Display));  // TODO: memory managment
    display->native_display = native_display;

    return (VC_Window) {display, *native_window , params};
}


VC_Window vc_create_window(VC_WindowParams *params) {
    return vc_create_window_x11(params);
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
    XNextEvent(display->native_display, &xevent);
    return vc_map_x11_event(xevent);
}

void vc_window_destroy_x11(VC_Window *window) {
    if (window->display != NULL) {
        if (window->native_window != 0) {
            XDestroyWindow(window->display->native_display, window->native_window);
        }
        XCloseDisplay(window->display->native_display);
    }
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

void vc_window_destroy(VC_Window *window) {
    vc_window_destroy_x11(window);
    free(window->display);
}

#endif // VC_C_IMPLEMENTATION

// ------------------------------------
// ------------------------------------
// ------------------------------------
// ------------------------------------

#if 1


void vc_handle_event(VC_Window *window, VC_EventType event_type) {
    (void) window;

    switch (event_type) {
        case VC_EVENT_EXPOSE:
            break;
        
        case VC_EVENT_KEY_PRESS:
            break;

        case VC_EVENT_UNKNOWN:
            break;

        default:
            break;
    }
}

int main() {
    VC_WindowParams params = {"My X11 Window", 800, 600};
        VC_Window window = vc_create_window(&params);
            vc_run_event_loop(&window, vc_handle_event);
        vc_window_destroy(&window);
    return 0;
}
#endif