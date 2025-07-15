#include "vulkan.h"
#include "window.h"

int main(void) {
    Window *win = lu_create_window("hello", 640, 480);

    while (true) {
        lu_draw_frame(win);
    }

    lu_terminate(win);
}

