#include "ttf.h"
#include "vulkan.h"
#include "window.h"
#include "arena.h"
#include "string.h"
#include "array.h"

int main(void) {
    Arena *app = lu_arena_init(512'000'000);

    Window *win = lu_create_window(app, S("bed"), 640, 480);

    //String font = S("/home/booz/ext/chinese/HanaMinB.ttf");
    //String font = S("/home/booz/ext/nerd-fonts/src/unpatched-fonts/Iosevka/Iosevka-Medium.ttf");
    //String font = S("/home/booz/ext/nerd-fonts/src/unpatched-fonts/Arimo/Regular/Arimo-Regular.ttf");
    String font = S("/home/booz/ext/nerd-fonts/src/unpatched-fonts/JetBrainsMono/Ligatures/Regular/JetBrainsMono-Regular.ttf");
    //String font = S("/home/booz/ext/nerd-fonts/src/unpatched-fonts/UbuntuMono/Regular/UbuntuMono-R.ttf");

    u8  c = 0;
    u32 t = 0;

    ArrayVec2 alphabet[26];

    Vertex vertices[] = {
        {{-0.5f, -0.5f},    {1.0f, 0.0f}},
        {{0.5f, -0.5f},     {0.0f, 0.0f}},
        {{0.5f, 0.5f},      {0.0f, 1.0f}},
        {{0.5f, 0.5f},      {0.0f, 1.0f}},
        {{-0.5f, 0.5f},     {1.0f, 1.0f}},
        {{-0.5f, -0.5f},    {1.0f, 0.0f}}
    };

    for (u16 i = 0; i < 26; i++) {
        alphabet[i] = lu_extract_glyph_from_font(app, font, 'a' + i);
    }

    //ArrayVec2 chinese = lu_extract_glyph_from_font(app, font, 0x21C1E);

    while (!lu_window_should_close(win)) {
        if (t > 1000) {
            t = 0;
            c = (c + 1) % 26;
        }

        //lu_create_vertex_buffer(win, alphabet[c]);
        lu_create_vertex_buffer(win, vertices, COUNT(vertices));
        lu_draw_frame(win, alphabet[c].len);
        lu_destroy_vertex_buffer(win);
        lu_poll_events(win);

        t += 1;
    }

    lu_terminate(win);
    lu_arena_free(app);
}

