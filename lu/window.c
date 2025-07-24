#include "window.h"
#include "vulkan.h"

static void xdg_surface_configure(
        void *data,
        struct xdg_surface *xdg_surface,
        u32 serial)
{
    xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

static void xdg_toplevel_configure(
        void *data,
        struct xdg_toplevel *xdg_toplevel,
        s32 width,
        s32 height,
        struct wl_array *states) 
{
    if (width == 0 || height == 0) return;

    // TODO: better to sync with compostor through surface configure callback

    CallbackData *cbd = data;

    cbd->win->width = width;
    cbd->win->height = height;
    lu_recreate_swapchain(cbd->arena, cbd->win);
}

static void xdg_toplevel_close(
        void *data,
        struct xdg_toplevel *toplevel)
{
    CallbackData *cbd = data;
    cbd->win->flags |= WINDOW_CLOSE_BIT;
}

static void xdg_wm_base_ping(
        void *data,
        struct xdg_wm_base *xdg_wm_base,
        u32 serial)
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
        u32 name, 
        const char *interface, 
        u32 version) 
{
    CallbackData *cbd = data;

    if        (lu_char_cmp(interface, wl_compositor_interface.name) == 0) {
        cbd->win->compositor = wl_registry_bind(wl_registry, name, &wl_compositor_interface, 4);
    } else if (lu_char_cmp(interface, xdg_wm_base_interface.name) == 0) {
        cbd->win->xdg_wm_base = wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(cbd->win->xdg_wm_base, &xdg_wm_base_listener, cbd);
    }
}

// TODO:
static void registry_global_remove(
        void *data, 
        struct wl_registry *wl_registry, 
        u32 name) 
{
    printf("registry global remove: name -> %d\n", name);
}

static const struct wl_registry_listener wl_registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

// TODO: error handling / recovery
void lu_setup_wl_window(Arena *arena, Window *win, String title) {
    CallbackData *cbd = (CallbackData*)lu_arena_alloc(arena, sizeof(CallbackData));
    cbd->win = win;
    cbd->arena = arena;

    win->display = wl_display_connect(NULL);
    win->registry = wl_display_get_registry(win->display);

    wl_registry_add_listener(win->registry, &wl_registry_listener, cbd);
    wl_display_roundtrip(win->display);

    win->surface = wl_compositor_create_surface(win->compositor);
    win->xdg_surface = xdg_wm_base_get_xdg_surface(win->xdg_wm_base, win->surface);
    win->xdg_toplevel = xdg_surface_get_toplevel(win->xdg_surface);

    xdg_surface_add_listener(win->xdg_surface, &xdg_surface_listener, cbd);
    xdg_toplevel_set_title(win->xdg_toplevel, title.value);
    xdg_toplevel_add_listener(win->xdg_toplevel, &xdg_toplevel_listener, cbd);
    wl_surface_commit(win->surface);
}

Window *lu_create_window(Arena *arena, String title, u16 width, u16 height) {
    Window *win = (Window*)lu_arena_alloc(arena, sizeof(Window));
    win->width = width;
    win->height = height;

    lu_setup_wl_window(arena, win, title);
    lu_setup_renderer(arena, win, title);

    return win;
}

inline bool lu_window_should_close(Window *win) {
    return (win->flags & WINDOW_CLOSE_BIT) == WINDOW_CLOSE_BIT;
}

void lu_poll_events(Window *win) {
    wl_display_dispatch(win->display);
}

void lu_terminate(Window *win) {
    lu_free_renderer(win);

    wl_display_disconnect(win->display);
    xdg_toplevel_destroy(win->xdg_toplevel);
    xdg_surface_destroy(win->xdg_surface);
    xdg_wm_base_destroy(win->xdg_wm_base);
    wl_surface_destroy(win->surface);
    wl_compositor_destroy(win->compositor);
    wl_registry_destroy(win->registry);
}

