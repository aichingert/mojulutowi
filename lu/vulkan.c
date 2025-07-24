#include "vulkan.h"
#include "arena.h"
#include "math.h"
#include "mem.h"

#define VK_VALIDATION "VK_LAYER_KHRONOS_validation"

static PFN_vkGetInstanceProcAddr loader = NULL;

PFN_vkCreateInstance vkCreateInstance;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
PFN_vkCreateDevice vkCreateDevice;
PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR;
PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR ;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;
PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
PFN_vkDestroyInstance vkDestroyInstance;
PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR;
PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR;
PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;

static PFN_vkGetDeviceProcAddr device_loader = NULL;

PFN_vkGetDeviceQueue vkGetDeviceQueue;
PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
PFN_vkDestroyImageView vkDestroyImageView;
PFN_vkDestroyDevice vkDestroyDevice;
PFN_vkCreateImageView vkCreateImageView;
PFN_vkCreateShaderModule vkCreateShaderModule;
PFN_vkDestroyShaderModule vkDestroyShaderModule;
PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
PFN_vkDestroyPipeline vkDestroyPipeline;
PFN_vkCreateCommandPool vkCreateCommandPool;
PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
PFN_vkDestroyCommandPool vkDestroyCommandPool;
PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
PFN_vkCmdBindPipeline vkCmdBindPipeline;
PFN_vkCmdSetViewport vkCmdSetViewport;
PFN_vkCmdSetScissor vkCmdSetScissor;
PFN_vkCmdDraw vkCmdDraw;
PFN_vkEndCommandBuffer vkEndCommandBuffer;
PFN_vkCreateSemaphore vkCreateSemaphore;
PFN_vkDestroySemaphore vkDestroySemaphore;
PFN_vkCreateFence vkCreateFence;
PFN_vkDestroyFence vkDestroyFence;
PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
PFN_vkResetCommandBuffer vkResetCommandBuffer;
PFN_vkQueueSubmit vkQueueSubmit;
PFN_vkWaitForFences vkWaitForFences;
PFN_vkResetFences vkResetFences;
PFN_vkQueuePresentKHR vkQueuePresentKHR;
PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
PFN_vkCreateBuffer vkCreateBuffer;
PFN_vkDestroyBuffer vkDestroyBuffer;
PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
PFN_vkAllocateMemory vkAllocateMemory;
PFN_vkBindBufferMemory vkBindBufferMemory;
PFN_vkMapMemory vkMapMemory;
PFN_vkUnmapMemory vkUnmapMemory;
PFN_vkFreeMemory vkFreeMemory;
PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;

void load_instance_proc_addr() {
    // TODO: look for other names as well
    void *module = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    assert(module != NULL);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    loader = (PFN_vkGetInstanceProcAddr)dlsym(module, "vkGetInstanceProcAddr");
#pragma GCC diagnostic pop

    assert(loader != NULL);

    vkCreateInstance = (PFN_vkCreateInstance)loader((VkInstance)NULL, "vkCreateInstance");
    vkEnumerateInstanceLayerProperties = 
        (PFN_vkEnumerateInstanceLayerProperties)loader((VkInstance)NULL, "vkEnumerateInstanceLayerProperties");
    vkCreateDebugReportCallbackEXT = 
        (PFN_vkCreateDebugReportCallbackEXT)loader((VkInstance)NULL, "vkCreateDebugReportCallbackEXT");
}

void instance_load_vulkan(VkInstance instance) {
    device_loader = (PFN_vkGetDeviceProcAddr)loader(instance, "vkGetDeviceProcAddr");
    vkCreateDevice   = (PFN_vkCreateDevice)loader(instance, "vkCreateDevice");
    vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)loader(instance, "vkEnumeratePhysicalDevices");
    vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)loader(instance, "vkGetPhysicalDeviceProperties");
    vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)loader(instance, "vkGetPhysicalDeviceFeatures");
    vkGetPhysicalDeviceQueueFamilyProperties = 
        (PFN_vkGetPhysicalDeviceQueueFamilyProperties)loader(instance, "vkGetPhysicalDeviceQueueFamilyProperties");
    vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR)loader(instance, "vkCreateWaylandSurfaceKHR");
    vkGetPhysicalDeviceWaylandPresentationSupportKHR  = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR )loader(
            instance,
            "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
    vkGetPhysicalDeviceSurfaceFormatsKHR = 
        (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)loader(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    vkGetPhysicalDeviceSurfacePresentModesKHR =
        (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)loader(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR =
        (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)loader(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    vkDestroyDebugReportCallbackEXT =
        (PFN_vkDestroyDebugReportCallbackEXT)loader(instance, "vkDestroyDebugReportCallbackEXT");
    vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)loader(instance, "vkDestroySurfaceKHR");
    vkDestroyInstance = (PFN_vkDestroyInstance)loader(instance, "vkDestroyInstance");
    vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)loader(instance, "vkEnumerateDeviceExtensionProperties");
    vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)loader(instance, "vkGetPhysicalDeviceMemoryProperties");

    vkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR)
        loader(instance, "vkCmdBeginRenderingKHR");
    vkCmdEndRenderingKHR   = (PFN_vkCmdEndRenderingKHR) 
        loader(instance, "vkCmdEndRenderingKHR");

    if (!vkCmdBeginRenderingKHR || !vkCmdEndRenderingKHR) {
        assert(!"unable to load function pointers for dynamic rendering!");
    }
}

void device_load_vulkan(VkDevice device) {
    vkGetDeviceQueue = (PFN_vkGetDeviceQueue)device_loader(device, "vkGetDeviceQueue");
    vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)device_loader(device, "vkCreateSwapchainKHR");
    vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)device_loader(device, "vkDestroySwapchainKHR");
    vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)device_loader(device, "vkGetSwapchainImagesKHR");
    vkDestroyImageView = (PFN_vkDestroyImageView)device_loader(device, "vkDestroyImageView");
    vkDestroyDevice = (PFN_vkDestroyDevice)device_loader(device, "vkDestroyDevice");
    vkCreateImageView = (PFN_vkCreateImageView)device_loader(device, "vkCreateImageView");
    vkCreateShaderModule = (PFN_vkCreateShaderModule)device_loader(device, "vkCreateShaderModule");
    vkDestroyShaderModule = (PFN_vkDestroyShaderModule)device_loader(device, "vkDestroyShaderModule");
    vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)device_loader(device, "vkCreatePipelineLayout");
    vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)device_loader(device, "vkDestroyPipelineLayout");
    vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)device_loader(device, "vkCreateGraphicsPipelines");
    vkDestroyPipeline = (PFN_vkDestroyPipeline)device_loader(device, "vkDestroyPipeline");
    vkCreateCommandPool = (PFN_vkCreateCommandPool)device_loader(device, "vkCreateCommandPool");
    vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)device_loader(device, "vkAllocateCommandBuffers");
    vkDestroyCommandPool = (PFN_vkDestroyCommandPool)device_loader(device, "vkDestroyCommandPool");
    vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)device_loader(device, "vkBeginCommandBuffer");
    vkCmdBindPipeline = (PFN_vkCmdBindPipeline)device_loader(device, "vkCmdBindPipeline");
    vkCmdSetViewport = (PFN_vkCmdSetViewport)device_loader(device, "vkCmdSetViewport");
    vkCmdSetScissor = (PFN_vkCmdSetScissor)device_loader(device, "vkCmdSetScissor");
    vkCmdDraw = (PFN_vkCmdDraw)device_loader(device, "vkCmdDraw");
    vkEndCommandBuffer = (PFN_vkEndCommandBuffer)device_loader(device, "vkEndCommandBuffer");
    vkCreateSemaphore = (PFN_vkCreateSemaphore)device_loader(device, "vkCreateSemaphore");
    vkDestroySemaphore = (PFN_vkDestroySemaphore)device_loader(device, "vkDestroySemaphore");
    vkCreateFence = (PFN_vkCreateFence)device_loader(device, "vkCreateFence");
    vkDestroyFence = (PFN_vkDestroyFence)device_loader(device, "vkDestroyFence");
    vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)device_loader(device, "vkAcquireNextImageKHR");
    vkResetCommandBuffer = (PFN_vkResetCommandBuffer)device_loader(device, "vkResetCommandBuffer");
    vkQueueSubmit = (PFN_vkQueueSubmit)device_loader(device, "vkQueueSubmit");
    vkWaitForFences = (PFN_vkWaitForFences)device_loader(device, "vkWaitForFences");
    vkResetFences = (PFN_vkResetFences)device_loader(device, "vkResetFences");
    vkQueuePresentKHR = (PFN_vkQueuePresentKHR)device_loader(device, "vkQueuePresentKHR");
    vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)device_loader(device, "vkCmdPipelineBarrier");
    vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)device_loader(device, "vkDeviceWaitIdle");
    vkCreateBuffer = (PFN_vkCreateBuffer)device_loader(device, "vkCreateBuffer");
    vkDestroyBuffer = (PFN_vkDestroyBuffer)device_loader(device, "vkDestroyBuffer");
    vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)device_loader(device, "vkGetBufferMemoryRequirements");
    vkAllocateMemory = (PFN_vkAllocateMemory)device_loader(device, "vkAllocateMemory");
    vkBindBufferMemory = (PFN_vkBindBufferMemory)device_loader(device, "vkBindBufferMemory");
    vkMapMemory = (PFN_vkMapMemory)device_loader(device, "vkMapMemory");
    vkUnmapMemory = (PFN_vkUnmapMemory)device_loader(device, "vkUnmapMemory");
    vkFreeMemory = (PFN_vkFreeMemory)device_loader(device, "vkFreeMemory");
    vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)device_loader(device, "vkCmdBindVertexBuffers");
}

bool validation_layers_are_available() {
    u32 layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties available_layers[layer_count];
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

    for (u32 i = 0; i < layer_count; i++) {
        if (lu_char_cmp(available_layers[i].layerName, VK_VALIDATION) == 0) return true;
    }

    return false;
}

static VkBool32 VKAPI_CALL debug_callback(
        VkDebugReportFlagsEXT flags, 
        VkDebugReportObjectTypeEXT object_type, 
        u64 object, 
        size_t location, 
        s32 message_code, 
        const char* p_layer_prefix, 
        const char* p_message, 
        void* p_user_data)
{
    const char *log_i = (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) ? "ERROR"
        : (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)             ? "WARNING"
        : (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) ? "PERFORMANCE" 
        : "INFO";

    char msg[4096] = {0};
    snprintf(msg, COUNT(msg), "%s: %s\n", log_i, p_message);
    printf("%s", msg);

    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) assert(!"Validation error encountered!");
    return VK_FALSE;
}

VkDebugReportCallbackEXT register_debug_callback(VkInstance instance) {
    if (!vkCreateDebugReportCallbackEXT) {
		return NULL;
    }

	VkDebugReportCallbackCreateInfoEXT create_info = { 
        .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
        .flags = VK_DEBUG_REPORT_WARNING_BIT_EXT 
        | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT 
        | VK_DEBUG_REPORT_ERROR_BIT_EXT,
        .pfnCallback = debug_callback,
    };

	VkDebugReportCallbackEXT callback = {0};
	VK_CHECK(vkCreateDebugReportCallbackEXT(instance, &create_info, 0, &callback));

    return callback;
}

VkInstance lu_create_instance(String name) {
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = name.value,
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_4,
    };

    const char *extensions[] = {
        "VK_KHR_surface",
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,

        // TODO: debug def
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    };

    // TODO: debug def this
    const char *validation_layers[] = { VK_VALIDATION };
    assert(validation_layers_are_available() && "No validation layers are available");

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = COUNT(extensions),
        .ppEnabledExtensionNames = extensions,
        .enabledLayerCount = COUNT(validation_layers),
        .ppEnabledLayerNames = validation_layers,
    };

    VkInstance instance = {0};
    VK_CHECK(vkCreateInstance(&create_info, NULL, &instance));
    return instance;
}

void lu_create_surface(Window *win) {
    VkWaylandSurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .display = win->display,
        .surface = win->surface,
    };

    VK_CHECK(vkCreateWaylandSurfaceKHR(win->renderer.instance, &create_info, NULL, &win->renderer.surface));
}

u32 find_queue_family(VkPhysicalDevice device) {
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    for (u32 i = 0; i < queue_family_count; i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return i;
        }
    }

    return 10020002;
}

// TODO: improve picking - score or smth
// TODO: could have more extension checks
void pick_suitable_device(Window *win) {
    VkInstance instance = win->renderer.instance;
    VkSurfaceKHR surface = win->renderer.surface;

    u32 device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);

    assert(device_count != 0 && "Failed to find GPU with Vulkan support!");

    VkPhysicalDevice devices[device_count] = {};
    VkPhysicalDevice fallback = VK_NULL_HANDLE;
    vkEnumeratePhysicalDevices(instance, &device_count, devices);

    // TODO: this should probably become it's own function
    for (u32 i = 0; i < device_count; i++) {
        u32 queue_family = find_queue_family(devices[i]);
        if (queue_family == 10020002) continue;

        bool supported = vkGetPhysicalDeviceWaylandPresentationSupportKHR(devices[i], queue_family, win->display);
        if (!supported) continue;

        u32 present_mode_count = 0, format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], surface, &format_count, NULL);
        vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], surface, &present_mode_count, NULL);

        if (present_mode_count == 0 || format_count == 0) continue;

        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures   device_features;
        u32 property_count = 0;

        vkEnumerateDeviceExtensionProperties(devices[i], NULL, &property_count, NULL);
        VkExtensionProperties ext_props[property_count] = {};
        vkEnumerateDeviceExtensionProperties(devices[i], NULL, &property_count, ext_props);

        supported = false;

        for (u32 i = 0; i < property_count; i++) {
            if (lu_char_cmp("VK_KHR_dynamic_rendering", ext_props[i].extensionName) == 0) {
                supported = true;
                break;
            }
        }

        if (!supported) continue;

        vkGetPhysicalDeviceProperties(devices[i], &device_properties);
        vkGetPhysicalDeviceFeatures(devices[i], &device_features);

        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
            device_features.geometryShader) 
        {
            win->renderer.physical_device = devices[i];
            return;
        } else if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU &&
                device_features.geometryShader) {
            fallback = devices[i];
        }
    }

    if (fallback != VK_NULL_HANDLE) {
        win->renderer.physical_device = fallback;
        return;
    }

    assert(!"No suitable physical device found!");
}

void lu_create_device(Window *win, u32 *queue_family) { 
    VkPhysicalDevice physical = win->renderer.physical_device;
    *queue_family = find_queue_family(physical);
    f32 queue_priority = 1.0f;

    VkDeviceQueueCreateInfo queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = *queue_family,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };

    VkPhysicalDeviceFeatures device_features = {0};

    const char *device_extensions[] = { 
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // TODO: should be a define
        "VK_KHR_dynamic_rendering",
    };

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
        .dynamicRendering = VK_TRUE,
    };

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &dynamic_rendering_feature,
        .queueCreateInfoCount       = 1,
        .pQueueCreateInfos          = &queue_create_info,
        .pEnabledFeatures           = &device_features,
        .enabledExtensionCount      = COUNT(device_extensions),
        .ppEnabledExtensionNames    = device_extensions,
    };

    VK_CHECK(vkCreateDevice(physical, &create_info, NULL, &win->renderer.device));
}

void lu_create_swapchain(Window *win) {
    VkRenderer vk = win->renderer;
    u32 present_mode_count = 0, format_count = 0;

    vkGetPhysicalDeviceSurfaceFormatsKHR(vk.physical_device, vk.surface, &format_count, NULL);
    VkSurfaceFormatKHR formats[format_count] = {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(vk.physical_device, vk.surface, &format_count, formats);

    vkGetPhysicalDeviceSurfacePresentModesKHR(vk.physical_device, vk.surface, &present_mode_count, NULL);
    VkPresentModeKHR modes[present_mode_count] = {};
    vkGetPhysicalDeviceSurfacePresentModesKHR(vk.physical_device, vk.surface, &present_mode_count, modes);

    VkSurfaceCapabilitiesKHR capabilities = {0};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk.physical_device, vk.surface, &capabilities);

    // TODO: might have to think about this
    VkExtent2D extent = {
        .width = win->width,
        .height = win->height,
    };

    u32 image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = vk.surface,
        .minImageCount = image_count,
        .imageExtent = extent,
        .imageFormat = formats[0].format,
        .imageColorSpace = formats[0].colorSpace,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = modes[0],
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    VkSwapchainKHR swapchain = {0};
    VK_CHECK(vkCreateSwapchainKHR(vk.device, &create_info, NULL, &swapchain));

    vkGetSwapchainImagesKHR(vk.device, swapchain, &image_count, NULL);
    VkImage *images = (VkImage*)malloc(sizeof(VkImage) * image_count);
    vkGetSwapchainImagesKHR(vk.device, swapchain, &image_count, images);

    win->renderer.format = formats[0].format;
    win->renderer.images = images;
    win->renderer.image_count = image_count;
    win->renderer.swapchain = swapchain;
}

void lu_create_image_views(Window *win) {
    VkRenderer vk = win->renderer;
    VkImageView *image_views = (VkImageView*)malloc(sizeof(VkImageView) * vk.image_count);

    for (u32 i = 0; i < vk.image_count; i ++) {
        VkImageViewCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = vk.images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = vk.format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        VK_CHECK(vkCreateImageView(vk.device, &create_info, NULL, &image_views[i]));
    }

    win->renderer.image_views = image_views;
}

void lu_destroy_swapchain(Window *win) {
    VkRenderer *vk = &win->renderer;

    VK_CHECK(vkDeviceWaitIdle(vk->device));

    for (u32 i = 0; i < vk->image_count; i++) {
        vkDestroyImageView(vk->device, vk->image_views[i], NULL);
    }

    free(vk->images);
    free(vk->image_views);
    vkDestroySwapchainKHR(vk->device, vk->swapchain, NULL);
}

// TODO: might have to recreate render pass as well
void lu_recreate_swapchain(Window *win) {
    lu_destroy_swapchain(win);

    lu_create_swapchain(win);
    lu_create_image_views(win);
}

VkShaderModule lu_create_shader_module(VkDevice device, const char *shader) {
    // TODO: error handling
    FILE *file = fopen(shader, "r");
    if (!file) {
        printf("wholy\n");
    }

    fseek(file, 0, SEEK_END);
    long long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    size_t size = file_size / sizeof(u8);
    u32 *buf = (u32*)malloc(file_size);
    fread(buf, sizeof(u32), size, file);
    fclose(file);

    VkShaderModuleCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = size, 
        .pCode = buf,
    };

    VkShaderModule shader_module = {0};
    VK_CHECK(vkCreateShaderModule(device, &create_info, NULL, &shader_module));
    free(buf);
    return shader_module;
}

VkVertexInputBindingDescription vertex_binding_description() {
    VkVertexInputBindingDescription description = {
        .binding = 0,
        .stride = sizeof(Vec2),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    return description;
}

VkVertexInputAttributeDescription vertex_attribute_description() {
    VkVertexInputAttributeDescription description = {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vec2, x),
    };

    return description;
}

void lu_create_graphics_pipeline(Window *win) {
    VkRenderer vk = win->renderer;

    VkShaderModule vert = lu_create_shader_module(vk.device, "lu/spirv/text.vert.spv");
    VkShaderModule frag = lu_create_shader_module(vk.device, "lu/spirv/text.frag.spv");

    VkPipelineShaderStageCreateInfo shader_stages[] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vert,
            .pName = "main",
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = frag,
            .pName = "main",
        }
    };

    VkDynamicState dynamic[] = { 
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = COUNT(dynamic),
        .pDynamicStates = dynamic,
    };

    VkVertexInputBindingDescription v_b_desc = vertex_binding_description();
    VkVertexInputAttributeDescription v_a_desc = vertex_attribute_description();

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .vertexAttributeDescriptionCount = 1,
        .pVertexBindingDescriptions = &v_b_desc,
        .pVertexAttributeDescriptions = &v_a_desc,
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (f32)win->width,
        .height = (f32)win->height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    VkRect2D scissor = {
        .offset = {0, 0},
        // TODO: might have to think about this
        .extent = { .width = win->width, .height = win->height },
    };

    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f,
    };

    VkPipelineColorBlendAttachmentState color_blend_attachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT
            | VK_COLOR_COMPONENT_G_BIT
            | VK_COLOR_COMPONENT_B_BIT
            | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
    };

    VkPipelineColorBlendStateCreateInfo color_blending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment,
    };

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    };

    VkPipelineLayout layout;
    VK_CHECK(vkCreatePipelineLayout(vk.device, &pipeline_layout_info, NULL, &layout));

    VkPipelineRenderingCreateInfoKHR pipeline_rendering_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &vk.format,
    };

    VkGraphicsPipelineCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &pipeline_rendering_info,
        .stageCount = COUNT(shader_stages),
        .pStages = shader_stages,
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = NULL,
        .pColorBlendState = &color_blending,
        .pDynamicState = &dynamic_state,
        .layout = layout,
    };

    VkPipeline graphics_pipeline = {0};
    VK_CHECK(vkCreateGraphicsPipelines(vk.device, VK_NULL_HANDLE, 1, &create_info, NULL, &graphics_pipeline));

    win->renderer.pipeline_layout = layout;
    win->renderer.graphics_pipeline = graphics_pipeline;
    vkDestroyShaderModule(vk.device, vert, NULL);
    vkDestroyShaderModule(vk.device, frag, NULL);
}

void lu_create_command_structures(Window *win) {
    VkRenderer *vk = &win->renderer;

    u32 queue_family = find_queue_family(vk->physical_device);

    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queue_family,
    };

    VK_CHECK(vkCreateCommandPool(vk->device, &pool_info, NULL, &vk->command_pool));

    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vk->command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = MAX_FRAMES_BETWEEN,
    };

    VK_CHECK(vkAllocateCommandBuffers(vk->device, &alloc_info, vk->command_buffers));
}

void lu_create_sync_objs(Window *win) {
    VkSemaphoreCreateInfo sema_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    VkRenderer *vk = &win->renderer;
    for (u32 i = 0; i < MAX_FRAMES_BETWEEN; i++) {
        VK_CHECK(vkCreateSemaphore(vk->device, &sema_info, NULL, &vk->acq_semas[i]));
        VK_CHECK(vkCreateSemaphore(vk->device, &sema_info, NULL, &vk->rel_semas[i]));
        VK_CHECK(vkCreateFence(vk->device, &fence_info, NULL, &vk->between_fences[i]));
    }
}

u32 find_memory_type(
        VkPhysicalDevice physical_device, 
        u32 type_filter, 
        VkMemoryPropertyFlags props) 
{
    VkPhysicalDeviceMemoryProperties mem_props = {0};
    vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_props);

    for (u32 i = 0; i < mem_props.memoryTypeCount; i++) {
        bool has_properties = (mem_props.memoryTypes[i].propertyFlags & props) == props;

        if (type_filter & (1 << i) && has_properties) {
            return i;
        }
    }

    assert(false && "Failed to find suitable memory type");
    return 0;
}

void lu_create_vertex_buffer(Window *win, ArrayVec2 vertices) {
    VkRenderer *vk = &win->renderer;

    VkBufferCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = ArrayItemSize(vertices) * vertices.len,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    VkBuffer vertex_buffer = {0};
    VK_CHECK(vkCreateBuffer(vk->device, &create_info, NULL, &vertex_buffer));

    VkMemoryRequirements mem_requires = {0};
    vkGetBufferMemoryRequirements(vk->device, vertex_buffer, &mem_requires);

    VkMemoryPropertyFlags props = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_requires.size,
        .memoryTypeIndex = find_memory_type(vk->physical_device, mem_requires.memoryTypeBits, props),
    };

    VkDeviceMemory vertex_buffer_memory = {0};
    VK_CHECK(vkAllocateMemory(vk->device, &alloc_info, NULL, &vertex_buffer_memory));

    vkBindBufferMemory(vk->device, vertex_buffer, vertex_buffer_memory, 0);

    void *data = NULL;
    vkMapMemory(vk->device, vertex_buffer_memory, 0, create_info.size, 0, &data);
    lu_memcpy(data, vertices.v, create_info.size);
    vkUnmapMemory(vk->device, vertex_buffer_memory);

    vk->vertex_buffer = vertex_buffer;
    vk->vertex_buffer_memory = vertex_buffer_memory;
}

void lu_destroy_vertex_buffer(Window *win) {
    VK_CHECK(vkDeviceWaitIdle(win->renderer.device));
    vkDestroyBuffer(win->renderer.device, win->renderer.vertex_buffer, NULL);
    vkFreeMemory(win->renderer.device, win->renderer.vertex_buffer_memory, NULL);
}

void lu_setup_renderer(Window *win, String name) {
    load_instance_proc_addr();
    VkInstance instance = lu_create_instance(name);
    instance_load_vulkan(instance);

    win->renderer.callback = register_debug_callback(instance);
    win->renderer.instance = instance;
    win->renderer.current_frame = 0;

    lu_create_surface(win);
    pick_suitable_device(win);

    u32 queue_family = 0;
    lu_create_device(win, &queue_family);
    device_load_vulkan(win->renderer.device);

    vkGetDeviceQueue(win->renderer.device, queue_family, 0, &win->renderer.graphics_queue);

    lu_create_swapchain(win);
    lu_create_image_views(win);
    lu_create_graphics_pipeline(win);
    lu_create_command_structures(win);
    lu_create_sync_objs(win);
}

void lu_record_command_buffer(
        Window *win, 
        VkCommandBuffer command_buffer, 
        u32 image_index,
        size_t vertices) 
{
    VkRenderer *vk = &win->renderer;

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    VK_CHECK(vkBeginCommandBuffer(command_buffer, &begin_info));

    // TODO: research some fields more
    VkImageMemoryBarrier image_to_draw_barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .image = vk->images[image_index],
        .subresourceRange = {
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .baseMipLevel = 0,
          .levelCount = 1,
          .baseArrayLayer = 0,
          .layerCount = 1,
        }
    };
    vkCmdPipelineBarrier(
        command_buffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0, 0, NULL, 0, NULL, 1,
        &image_to_draw_barrier
    );

    VkClearValue clear_color = {{{ 0x18 / 255.0f, 0x16 / 255.0f, 0x16 / 255.0f, 1.0f }}};

    VkRenderingAttachmentInfoKHR color_attachment_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .imageView = vk->image_views[image_index],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = clear_color,
    };

    VkRenderingInfoKHR rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
        .renderArea = {
            .offset = { 0, 0 },
            // TODO: might have to think about this
            .extent = { win->width, win->height },
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_info,
    };

    vkCmdBeginRenderingKHR(command_buffer, &rendering_info);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk->graphics_pipeline);

    VkBuffer vertex_buffers[] = {vk->vertex_buffer};
    VkDeviceSize offsets[]    = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (f32)win->width,
        .height = (f32)win->height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor = {
        .offset = { 0, 0 },
        // TODO: have to think about this
        .extent = { .width = win->width, .height = win->height },
    };
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    vkCmdDraw(command_buffer, vertices, 1, 0, 0);

    vkCmdEndRenderingKHR(command_buffer);

    VkImageMemoryBarrier image_memory_barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .image = win->renderer.images[image_index],
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    vkCmdPipelineBarrier(
        command_buffer, 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0, 0, NULL, 0, NULL, 1,
        &image_memory_barrier
    );
    VK_CHECK(vkEndCommandBuffer(command_buffer));
}

void lu_draw_frame(Window *win, size_t vertices) {
    VkRenderer *vk = &win->renderer;
    u32 frame = vk->current_frame;

    vkWaitForFences(vk->device, 1, &vk->between_fences[frame], VK_TRUE, UINT64_MAX);
    vkResetFences(vk->device, 1, &vk->between_fences[frame]);

    u32 image_index = 0;
    vkAcquireNextImageKHR(vk->device, vk->swapchain, UINT64_MAX, vk->acq_semas[frame], VK_NULL_HANDLE, &image_index);

    vkResetCommandBuffer(vk->command_buffers[frame], 0);
    lu_record_command_buffer(win, vk->command_buffers[frame], image_index, vertices);

    VkSemaphore wait_semas[] = {vk->acq_semas[frame]};
    VkSemaphore signal_semas[] = { vk->rel_semas[frame] };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = COUNT(wait_semas),
        .pWaitSemaphores = wait_semas,
        .pWaitDstStageMask = wait_stages,
        .commandBufferCount = 1, 
        .pCommandBuffers = &vk->command_buffers[frame],
        .signalSemaphoreCount = COUNT(signal_semas),
        .pSignalSemaphores = signal_semas,
    };

    VK_CHECK(vkQueueSubmit(vk->graphics_queue, 1, &submit_info, vk->between_fences[frame]));

    VkSwapchainKHR swapchains[] = {vk->swapchain};
    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signal_semas,
        .swapchainCount = COUNT(swapchains),
        .pSwapchains = swapchains,
        .pImageIndices = &image_index,
    };

    vkQueuePresentKHR(vk->graphics_queue, &present_info);
    vk->current_frame = (frame + 1) % MAX_FRAMES_BETWEEN;
}

void lu_free_renderer(Window *win) {
    VkRenderer *vk = &win->renderer;

    lu_destroy_swapchain(win);
    lu_destroy_vertex_buffer(win);

    for (u32 i = 0; i < MAX_FRAMES_BETWEEN; i++) {
        vkDestroySemaphore(vk->device, vk->acq_semas[i], NULL);
        vkDestroySemaphore(vk->device, vk->rel_semas[i], NULL);
        vkDestroyFence(vk->device, vk->between_fences[i], NULL);
    }

    // TODO: ifdef debug
    if (vkCreateDebugReportCallbackEXT) {
        vkDestroyDebugReportCallbackEXT(vk->instance, vk->callback, NULL);
    }

    vkDestroyCommandPool(vk->device, vk->command_pool, NULL);
    vkDestroyPipeline(vk->device, vk->graphics_pipeline, NULL);
    vkDestroyPipelineLayout(vk->device, vk->pipeline_layout, NULL);
    vkDestroyDevice(vk->device, NULL);
    vkDestroySurfaceKHR(vk->instance, vk->surface, NULL);
    vkDestroyInstance(vk->instance, NULL);
}
