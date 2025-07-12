#include "window.h"

static void wl_buffer_release(
        void *data, 
        struct wl_buffer *wl_buffer) 
{
    wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
    .release = wl_buffer_release,
};

static void randname(char *buf) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    long r = ts.tv_nsec;

    for (int i = 0; i < 6; ++i) {
        buf[i] = 'A'+(r&15)+(r&16)*2;
        r >>= 5;
    }
}

static int create_shm_file(void) {
    int retries = 100;

    do {
        char name[] = "/wl_shm-XXXXXX";
        randname(name + sizeof(name) - 7);

        --retries;
        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            shm_unlink(name);
            return fd;
        }
    } while (retries > 0 && errno == EEXIST);

    return -1;
}

static int allocate_shm_file(size_t size) {
    int fd = create_shm_file();

    if (fd < 0)
        return -1;

    int ret;

    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);

    if (ret < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

static struct wl_buffer *draw_frame(Window *win) {
    const int width = (int)win->width;
    const int height = (int)win->height;
    int stride = 4 * width;
    int size = stride * height;

    int fd = allocate_shm_file(size);
    if (fd == -1) return NULL;

    uint32_t *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        return NULL;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool(win->shm, fd, size);
    struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    munmap(data, size);
    wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
    return buffer;
}

static void xdg_surface_configure(
        void *data,
        struct xdg_surface *xdg_surface,
        uint32_t serial)
{
    Window *win = data;

    xdg_surface_ack_configure(xdg_surface, serial);

    // TODO change buffer strategy
    printf("surface is ready for the next frame\n");

    struct wl_buffer *buffer = draw_frame(win);
    wl_surface_attach(win->surface, buffer, 0, 0);
    wl_surface_commit(win->surface);
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

    return win;
}

int lu_poll_events(Window *win) {
    return wl_display_dispatch(win->display);
}


