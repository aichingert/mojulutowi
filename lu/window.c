#include "window.h"
#include "vulkan.h"

static void xdg_surface_configure(
        void *data,
        struct xdg_surface *xdg_surface,
        uint32_t serial)
{
    xdg_surface_ack_configure(xdg_surface, serial);

    printf("surface is ready for the next frame\n");

    /*
    struct wl_buffer *buffer = draw_frame(win);
    wl_surface_attach(win->surface, buffer, 0, 0);
    wl_surface_commit(win->surface);
    */
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

static void xdg_toplevel_configure(
        void *data,
        struct xdg_toplevel *xdg_toplevel,
        int32_t width,
        int32_t height,
        struct wl_array *states) 
{
    if (width == 0 || height == 0) return;

    Window *win = data;
    win->width = width;
    win->height = height;

    // TODO: update vulkan surface as well
}

static void xdg_toplevel_close(
        void *data,
        struct xdg_toplevel *toplevel)
{
    // TODO: set close = true
}

static void xdg_wm_base_ping(
        void *data,
        struct xdg_wm_base *xdg_wm_base,
        uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_configure,
    .close = xdg_toplevel_close,
};

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

static void registry_global(
        void *data, 
        struct wl_registry *wl_registry, 
        uint32_t name, 
        const char *interface, 
        uint32_t version) 
{
    Window *window = data;

    if        (strcmp(interface, wl_shm_interface.name) == 0) {
        // TODO: use linux dmabuf
        window->shm = wl_registry_bind(wl_registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface, wl_compositor_interface.name) == 0) {
        window->compositor = wl_registry_bind(wl_registry, name, &wl_compositor_interface, 4);
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        window->xdg_wm_base = wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(window->xdg_wm_base, &xdg_wm_base_listener, window);
    }
}

// TODO:
static void registry_global_remove(
        void *data, 
        struct wl_registry *wl_registry, 
        uint32_t name) 
{
    printf("registry global remove: name -> %d\n", name);
}

static const struct wl_registry_listener wl_registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

// TODO: error handling / recovery
void lu_setup_wl_window(Window *win, const char *title) {
    win->display = wl_display_connect(NULL);
    win->registry = wl_display_get_registry(win->display);
    wl_registry_add_listener(win->registry, &wl_registry_listener, win);
    wl_display_roundtrip(win->display);

    win->surface = wl_compositor_create_surface(win->compositor);
    win->xdg_surface = xdg_wm_base_get_xdg_surface(win->xdg_wm_base, win->surface);
    win->xdg_toplevel = xdg_surface_get_toplevel(win->xdg_surface);
    xdg_surface_add_listener(win->xdg_surface, &xdg_surface_listener, win);
    xdg_toplevel_set_title(win->xdg_toplevel, title);
    xdg_toplevel_add_listener(win->xdg_toplevel, &xdg_toplevel_listener, win);
    wl_surface_commit(win->surface);
}

Window *lu_create_window(
        const char *title, 
        uint16_t width, 
        uint16_t height) 
{
    Window *win = (Window*)malloc(sizeof(Window));
    win->width = width;
    win->height = height;

    lu_setup_wl_window(win, title);
    lu_setup_renderer(win, title);

    return win;
}

int lu_poll_events(Window *win) {
    return wl_display_dispatch(win->display);
}

void lu_terminate(Window *win) {
    lu_free_renderer(win);
    free(win);
}


