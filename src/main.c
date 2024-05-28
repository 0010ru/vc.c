#include <stdio.h>
#include <stdlib.h>

#include "../include/vc_core.h"
#include "../include/vc_arena.h"

void handle_event(VC_Window *window, VC_EventType event_type)
{

}

int main() {
    VC_Arena *arena = vc_arena_create(1024 * 1024);

    VC_WindowParams params = {"Main Window", 800, 600};
    VC_Window window = vc_create_window(&params, arena);
    vc_run_event_loop(&window, handle_event);
    
    vc_window_destroy(&window);
    vc_arena_destroy(arena);
    return 0;
}
