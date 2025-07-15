#include "vulkan.h"
#include "window.h"

int main(void) {
    Window *win = lu_create_window("hello", 640, 480);

    while (lu_poll_events(win)) {
        lu_draw_frame(win);
    }

    lu_terminate(win);
}

