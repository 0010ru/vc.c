#ifndef VC_CORE_H
#define VC_CORE_H

#include <stdint.h>
#include <X11/Xlib.h>
#include "../include/vc_arena.h"

// --- Event Types ---

typedef enum {
    VC_EVENT_UNKNOWN,
    VC_EVENT_EXPOSE,
    VC_EVENT_KEY_PRESS,
} VC_EventType;

// --- Window Parameters ---

typedef struct {
    const char *name;
    uint16_t width;
    uint16_t height;
} VC_WindowParams;

// --- Native Windowing ---

typedef Display NativeDisplay;
typedef Window NativeWindow;

// --- Display ---

typedef struct {
    NativeDisplay *native_display;
} VC_Display;

// --- Window ---

typedef struct {
    VC_Display *display;
    NativeWindow native_window;
    VC_WindowParams *params;
} VC_Window;

// --- Event Handling ---

typedef void (*VC_F_HandleEvent)(VC_Window *window, VC_EventType event_type);

// --- Public API ---

VC_Window vc_create_window(VC_WindowParams *params, VC_Arena *arena);
VC_EventType vc_get_next_event(VC_Window *window);
void vc_run_event_loop(VC_Window *window, VC_F_HandleEvent vc_handle_event);
void vc_window_destroy(VC_Window *window);

#endif // VC_CORE_H