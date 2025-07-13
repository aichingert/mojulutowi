#ifndef LU_WINDOW_H
#define LU_WINDOW_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "xdg-shell.h"

#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.h>

typedef struct VkRenderer {
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkSwapchainKHR swapchain;
    VkFormat    format;
    VkImage     *images;
    VkImageView *image_views;
    uint32_t    image_count;

    // TODO: ifdef debug
    VkDebugReportCallbackEXT callback;
} VkRenderer;

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

    VkRenderer renderer;
} Window;

Window *lu_create_window(const char *title, uint16_t width, uint16_t height);
int     lu_poll_events(Window *win);
void    lu_terminate(Window *win);

#endif /* LU_WINDOW_H */
