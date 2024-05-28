#include <stdio.h>
#include <stdlib.h>

#include "../include/vc_core.h"
#include "../include/vc_arena.h"

void handle_event(VC_Window *window, VC_EventType event_type)
{

}

int main() {
    VC_Arena *arena = vc_arena_create(1024 * 1024);
    VC_WindowParams *params = (VC_WindowParams *)vc_arena_alloc(arena, sizeof(VC_WindowParams));
    if (params == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for window parameters!\n");
        return EXIT_FAILURE;
    }

    params->width = 800;
    params->height = 600;
    params->name = "Main Window";

    VC_Window window = vc_create_window(params, arena);
    vc_run_event_loop(&window, handle_event);
    vc_window_destroy(&window);

    vc_arena_destroy(arena);
    return 0;
}
