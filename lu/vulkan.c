#include "vulkan.h"

PFN_vkCreateInstance vkCreateInstance;

void load_vulkan() {
    // TODO: look for other names as well
    void *module = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    assert(module != NULL);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    PFN_vkGetInstanceProcAddr loader = (PFN_vkGetInstanceProcAddr)dlsym(module, "vkGetInstanceProcAddr");
#pragma GCC diagnostic pop

    vkCreateInstance = (PFN_vkCreateInstance)loader((VkInstance)NULL, "vkCreateInstance");
}


VkInstance lu_create_instance() {
    load_vulkan();

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    };

    VkInstance instance = {0};
    VK_CHECK(vkCreateInstance(&create_info, NULL, &instance));
    return instance;
}

VkDevice lu_create_device() {
    VkDevice device = {0};
    return device;
}
