#include "window.h"


void lu_setup_wl_window(Window *win) {
    win->display = wl_display_connect(NULL);
    // win->compositor = 
}

Window lu_create_window(const char *title, uint16_t width, uint16_t height) {
    Window win = {
        .display = NULL,
    };

    lu_setup_wl_window(&win);

    return win;
}



