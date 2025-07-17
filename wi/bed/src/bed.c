#include "vulkan.h"
#include "window.h"
#include "ttf.h"

int main(void) {
    //Window *win = lu_create_window("hello", 640, 480);

    lu_parse_ttf("/home/booz/ext/nerd-fonts/src/unpatched-fonts/Iosevka/Iosevka-Medium.ttf");
    //lu_parse_ttf("/home/booz/ext/nerd-fonts/src/unpatched-fonts/Arimo/Regular/Arimo-Regular.ttf");

    /*
    while (!lu_window_should_close(win)) {
        //lu_draw_frame(win);
        lu_poll_events(win);
    }
    */

    //lu_terminate(win);
}

