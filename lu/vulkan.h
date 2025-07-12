#ifndef LU_VULKAN_H
#define LU_VULKAN_H

#include <dlfcn.h>
#include <assert.h>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#define VK_CHECK(call) \
    do \
    { \
        VkResult result_ = call; \
        assert(result_ == VK_SUCCESS); \
    } while(0)

VkInstance  lu_create_instance();
VkDevice    lu_create_device();

#endif /* LU_VULKAN_H */
