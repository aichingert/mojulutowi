#ifndef LU_VULKAN_H
#define LU_VULKAN_H

#include "lu.h"
#include "window.h"

#include <dlfcn.h>
#include <assert.h>

#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.h>

#define VK_CHECK(call) \
    do \
    { \
        VkResult result_ = call; \
        if (result_ != VK_SUCCESS) { \
            printf("VK_ERROR: %d\n", result_); \
        } \
        assert(result_ == VK_SUCCESS); \
    } while(0)


void        lu_setup_renderer(Window *win, const char *name);
void        lu_free_renderer(Window *win);

#endif /* LU_VULKAN_H */
