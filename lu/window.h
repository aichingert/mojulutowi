#ifndef LU_WINDOW_H
#define LU_WINDOW_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "xdg-shell.h"

typedef struct Window {
    struct wl_shm *shm;
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_surface *surface;

    struct xdg_wm_base *xdg_wm_base;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;

    int32_t width;
    int32_t height;
} Window;

Window *lu_create_window(const char *title, uint16_t width, uint16_t height);
int    lu_poll_events(Window *win);

#endif /* LU_WINDOW_H */
