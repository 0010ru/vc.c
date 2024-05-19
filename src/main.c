#include "vc.h"

int main() {
    VC_WindowParams params = {"My X11 Window", 800, 600};
    VC_Window window = vc_create_window(&params);
    vc_run_event_loop(&window);
    vc_window_destroy(&window);
    return 0;
}