#ifndef VC_H
#define VC_H

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

#endif
