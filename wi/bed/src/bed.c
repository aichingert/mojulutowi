#include "ttf.h"
#include "vulkan.h"
#include "window.h"

int main(void) {
    Window *win = lu_create_window("hello", 640, 480);

    size_t len = 0;
    const char *font = "/home/booz/ext/nerd-fonts/src/unpatched-fonts/Iosevka/Iosevka-Medium.ttf";
    // /home/booz/ext/nerd-fonts/src/unpatched-fonts/Arimo/Regular/Arimo-Regular.ttf
    //const char *font = "/home/booz/ext/nerd-fonts/src/unpatched-fonts/Arimo/Regular/Arimo-Regular.ttf";
    // TODO: jetbrains mono
    //const char *font = "/home/booz/ext/nerd-fonts/src/unpatched-fonts/JetBrainsMono/Ligatures/Regular/JetBrainsMono-Regular.ttf";
    //const char *font = "/home/booz/ext/nerd-fonts/src/unpatched-fonts/UbuntuMono/Regular/UbuntuMono-R.ttf";

    
    printf("%lu\n", len);
    u8  c = 0;
    u16 t = 0;

    while (!lu_window_should_close(win)) {
        if (t > 1000) {
            t = 0;
            c = (c + 1) % 26;
        }

        Vertex *vertices = lu_extract_glyph_from_font(font, 'A' + c, &len);
        lu_create_vertex_buffer(win, vertices, len);
        lu_draw_frame(win, len);
        lu_destroy_vertex_buffer(win);
        //free(vertices);
        lu_poll_events(win);

        t += 1;
    }

    lu_terminate(win);
}

