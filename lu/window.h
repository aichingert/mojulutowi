#ifndef LU_WINDOW_H
#define LU_WINDOW_H

#include <stdint.h>
#include "wayland.h"

typedef struct Window {
    struct wl_display *display;
    struct wl_compositor *compositor;

} Window;

Window lu_create_window(const char *title, uint16_t width, uint16_t height);

#endif /* LU_WINDOW_H */
