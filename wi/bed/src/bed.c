#include "ttf.h"
#include "vulkan.h"
#include "window.h"

int main(void) {
    Window *win = lu_create_window("hello", 640, 480);

    size_t len = 0;
    // ("/home/booz/ext/nerd-fonts/src/unpatched-fonts/Arimo/Regular/Arimo-Regular.ttf")
    const char *font = "/home/booz/ext/nerd-fonts/src/unpatched-fonts/Iosevka/Iosevka-Medium.ttf";

    Vertex *vertices = lu_extract_glyph_from_font(font, 'A', &len);
    lu_create_vertex_buffer(win, vertices, len);
    printf("%lu\n", len);

    while (!lu_window_should_close(win)) {
        lu_draw_frame(win, len);
        lu_poll_events(win);
    }

    free(vertices);
    lu_terminate(win);
}

