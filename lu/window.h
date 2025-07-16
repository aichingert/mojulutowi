#ifndef LU_WINDOW_H
#define LU_WINDOW_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lu.h"

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
    u32             image_count;

    VkQueue             graphics_queue;
    VkRenderPass        render_pass;
    VkPipelineLayout    pipeline_layout;
    VkPipeline          graphics_pipeline;

    VkCommandPool   command_pool;
    VkCommandBuffer command_buffers[MAX_FRAMES_BETWEEN];

    VkSemaphore acq_semas[MAX_FRAMES_BETWEEN];
    VkSemaphore rel_semas[MAX_FRAMES_BETWEEN];
    VkFence between_fences[MAX_FRAMES_BETWEEN];

    u32 current_frame;

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

    u8  flags;
    u32 width;
    u32 height;

    VkRenderer renderer;
} Window;

enum WindowFlags {
    WINDOW_CLOSE_BIT = 1 << (0),
};

Window  *lu_create_window(const char *title, u16 width, u16 height);
bool    lu_window_should_close(Window *win);
void    lu_poll_events(Window *win);
void    lu_terminate(Window *win);

#endif /* LU_WINDOW_H */
