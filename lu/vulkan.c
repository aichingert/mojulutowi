#include "vulkan.h"

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

static PFN_vkGetDeviceProcAddr device_loader = NULL;

PFN_vkGetDeviceQueue vkGetDeviceQueue;
PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
PFN_vkDestroyImageView vkDestroyImageView;
PFN_vkDestroyDevice vkDestroyDevice;
PFN_vkCreateImageView vkCreateImageView;

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
}

void device_load_vulkan(VkDevice device) {
    vkGetDeviceQueue = (PFN_vkGetDeviceQueue)device_loader(device, "vkGetDeviceQueue");
    vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)device_loader(device, "vkCreateSwapchainKHR");
    vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)device_loader(device, "vkDestroySwapchainKHR");
    vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)device_loader(device, "vkGetSwapchainImagesKHR");
    vkDestroyImageView = (PFN_vkDestroyImageView)device_loader(device, "vkDestroyImageView");
    vkDestroyDevice = (PFN_vkDestroyDevice)device_loader(device, "vkDestroyDevice");
    vkCreateImageView = (PFN_vkCreateImageView)device_loader(device, "vkCreateImageView");
}

bool validation_layers_are_available() {
    uint32_t layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties available_layers[layer_count];
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

    for (uint32_t i = 0; i < layer_count; i++) {
        if (strcmp(available_layers[i].layerName, VK_VALIDATION)) return true;
    }

    return false;
}

static VkBool32 VKAPI_CALL debug_callback(
        VkDebugReportFlagsEXT flags, 
        VkDebugReportObjectTypeEXT object_type, 
        uint64_t object, 
        size_t location, 
        int32_t message_code, 
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

VkInstance lu_create_instance(const char *name) {
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = name,
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
    assert(!validation_layers_are_available() && "No validation layers are available");

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

VkDebugReportCallbackEXT register_debug_callback(VkInstance instance) {
    if (!vkCreateDebugReportCallbackEXT)
		return NULL;

	VkDebugReportCallbackCreateInfoEXT create_info = { 
        .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
        .flags = VK_DEBUG_REPORT_WARNING_BIT_EXT 
        | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT 
        | VK_DEBUG_REPORT_ERROR_BIT_EXT,
        .pfnCallback = debug_callback,
    };

	VkDebugReportCallbackEXT callback = 0;
	VK_CHECK(vkCreateDebugReportCallbackEXT(instance, &create_info, 0, &callback));
}

void lu_create_surface(Window *win) {
    VkWaylandSurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .display = win->display,
        .surface = win->surface,
    };

    VK_CHECK(vkCreateWaylandSurfaceKHR(win->renderer.instance, &create_info, NULL, &win->renderer.surface));
}

uint32_t find_queue_family(VkPhysicalDevice device) {
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    for (uint32_t i = 0; i < queue_family_count; i++) {
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

    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);

    assert(device_count != 0 && "Failed to find GPU with Vulkan support!");

    VkPhysicalDevice devices[device_count] = {};
    vkEnumeratePhysicalDevices(instance, &device_count, devices);

    for (uint32_t i = 0; i < device_count; i++) {
        uint32_t queue_family = find_queue_family(devices[i]);
        if (queue_family == 10020002) continue;

        bool supported = vkGetPhysicalDeviceWaylandPresentationSupportKHR(devices[i], queue_family, win->display);
        if (!supported) continue;

        uint32_t present_mode_count = 0, format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], surface, &format_count, NULL);
        vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], surface, &present_mode_count, NULL);

        if (present_mode_count == 0 || format_count == 0) continue;

        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures   device_features;

        vkGetPhysicalDeviceProperties(devices[i], &device_properties);
        vkGetPhysicalDeviceFeatures(devices[i], &device_features);

        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
            device_features.geometryShader) 
        {
            win->renderer.physical_device = devices[i];
            return;
        }
    }

    assert(!"No suitable physical device found!");
}

void lu_create_device(Window *win, uint32_t *queue_family) { 
    VkPhysicalDevice physical = win->renderer.physical_device;
    *queue_family = find_queue_family(physical);
    float    queue_priority = 1.0f;

    VkDeviceQueueCreateInfo queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = *queue_family,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };

    VkPhysicalDeviceFeatures device_features = {0};

    const char *device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount       = 1,
        .pQueueCreateInfos          = &queue_create_info,
        .pEnabledFeatures           = &device_features,
        .enabledExtensionCount      = COUNT(device_extensions),
        .ppEnabledExtensionNames    = device_extensions,
    };

    VK_CHECK(vkCreateDevice(physical, &create_info, NULL, &win->renderer.device));
}

void lu_create_swapchain(Window *win) {
    VkDevice device = win->renderer.device;
    VkPhysicalDevice physical = win->renderer.physical_device;
    VkSurfaceKHR surface = win->renderer.surface;

    uint32_t present_mode_count = 0, format_count = 0;

    vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &format_count, NULL);
    VkSurfaceFormatKHR formats[format_count] = {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &format_count, formats);

    vkGetPhysicalDeviceSurfacePresentModesKHR(physical, surface, &present_mode_count, NULL);
    VkPresentModeKHR modes[present_mode_count] = {};
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical, surface, &present_mode_count, modes);

    VkSurfaceCapabilitiesKHR capabilities = {0};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, surface, &capabilities);

    VkExtent2D extent = {
        .width = win->width,
        .height = win->height,
    };

    uint32_t image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
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
    VK_CHECK(vkCreateSwapchainKHR(device, &create_info, NULL, &swapchain));

    vkGetSwapchainImagesKHR(device, swapchain, &image_count, NULL);
    VkImage *images = (VkImage*)malloc(sizeof(VkImage) * image_count);
    vkGetSwapchainImagesKHR(device, swapchain, &image_count, images);

    win->renderer.format = formats[0].format;
    win->renderer.images = images;
    win->renderer.image_count = image_count;
}

void lu_create_image_views(Window *win) {
    VkRenderer vk = win->renderer;
    vk.image_views = (VkImageView*)malloc(sizeof(VkImageView) * vk.image_count);

    for (uint32_t i = 0; i < vk.image_count; i ++) {
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

        VK_CHECK(vkCreateImageView(vk.device, &create_info, NULL, &vk.image_views[i]));
    }
}

void lu_setup_renderer(Window *win, const char *name) {
    load_instance_proc_addr();
    VkInstance instance = lu_create_instance(name);
    instance_load_vulkan(instance);

    win->renderer.callback = register_debug_callback(instance);
    win->renderer.instance = instance;

    lu_create_surface(win);
    pick_suitable_device(win);

    uint32_t queue_family = 0;
    lu_create_device(win, &queue_family);
    device_load_vulkan(win->renderer.device);

    VkQueue graphics_queue = {0};
    vkGetDeviceQueue(win->renderer.device, queue_family, 0, &graphics_queue);

    lu_create_swapchain(win);

}

void lu_free_renderer(Window *win) {
    VkRenderer vk = win->renderer;

    for (uint32_t i = 0; i < vk.image_count; i++) {
        vkDestroyImageView(vk.device, vk.image_views[i], NULL);
    }

    // TODO: ifdef debug
    vkDestroyDebugReportCallbackEXT(vk.instance, vk.callback, NULL);

    vkDestroySwapchainKHR(vk.device, vk.swapchain, NULL);
    vkDestroyDevice(vk.device, NULL);
    vkDestroySurfaceKHR(vk.instance, vk.surface, NULL);
    vkDestroyInstance(vk.instance, NULL);
}
