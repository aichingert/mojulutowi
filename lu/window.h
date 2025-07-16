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

#define MAX_FRAMES_BETWEEN 2

typedef struct VkRenderer {
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkDevice device;

    VkFormat        format;
    VkSwapchainKHR  swapchain;
    VkImage         *images;
    VkImageView     *image_views;
    VkFramebuffer   *framebuffers;
    uint32_t        image_count;

    VkQueue             graphics_queue;
    VkRenderPass        render_pass;
    VkPipelineLayout    pipeline_layout;
    VkPipeline          graphics_pipeline;

    VkCommandPool   command_pool;
    VkCommandBuffer command_buffers[MAX_FRAMES_BETWEEN];

    VkSemaphore acq_semas[MAX_FRAMES_BETWEEN];
    VkSemaphore rel_semas[MAX_FRAMES_BETWEEN];
    VkFence between_fences[MAX_FRAMES_BETWEEN];

    uint32_t current_frame;

    // TODO: ifdef debug
    VkDebugReportCallbackEXT callback;
} VkRenderer;

typedef struct Window {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_surface *surface;

    struct xdg_wm_base *xdg_wm_base;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;

    uint8_t flags;
    int32_t width;
    int32_t height;

    VkRenderer renderer;
} Window;

enum WindowFlags {
    WINDOW_CLOSE_BIT = 1 << (0),
};

Window  *lu_create_window(const char *title, uint16_t width, uint16_t height);
bool    lu_window_should_close(Window *win);
void    lu_poll_events(Window *win);
void    lu_terminate(Window *win);

#endif /* LU_WINDOW_H */
