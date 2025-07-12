#include "vulkan.h"

#define VK_VALIDATION "VK_LAYER_KHRONOS_validation"

PFN_vkCreateInstance vkCreateInstance;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;

static PFN_vkGetInstanceProcAddr loader = NULL;

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

void load_vulkan_functions(VkInstance instance) {
    vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)loader(instance, "vkEnumeratePhysicalDevices");
    vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)loader(instance, "vkGetPhysicalDeviceProperties");
    vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)loader(instance, "vkGetPhysicalDeviceFeatures");
    vkGetPhysicalDeviceQueueFamilyProperties = 
        (PFN_vkGetPhysicalDeviceQueueFamilyProperties)loader(instance, "vkGetPhysicalDeviceQueueFamilyProperties");
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
        "VK_KHR_wayland_surface",
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

uint32_t find_queue_family(VkPhysicalDevice device) {
    uint32_t family_index = 10020002;
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    for (uint32_t i = 0; i < queue_family_count; i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            family_index = i;
        }
    }

    return family_index;
}

// TODO: improve picking - score or smth
VkPhysicalDevice pick_suitable_device(VkInstance instance) {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);

    assert(device_count != 0 && "Failed to find GPU with Vulkan support!");

    VkPhysicalDevice devices[device_count] = {};
    vkEnumeratePhysicalDevices(instance, &device_count, devices);

    for (uint32_t i = 0; i < device_count; i++) {
        if (find_queue_family(devices[i]) == 10020002) continue;

        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures   device_features;

        vkGetPhysicalDeviceProperties(devices[i], &device_properties);
        vkGetPhysicalDeviceFeatures(devices[i], &device_features);

        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
            device_features.geometryShader) 
        {
            return devices[i];
        }
    }

    return VK_NULL_HANDLE;
}

VkDevice lu_create_device(VkInstance instance) {
    VkPhysicalDevice physical_device = pick_suitable_device(instance);
    assert(physical_device != VK_NULL_HANDLE && "No suitable device found!");

    VkDevice device = {0};
    return device;
}

void lu_setup_vulkan(Window *win, const char *name) {
    load_instance_proc_addr();
    VkInstance instance = lu_create_instance(name);
    load_vulkan_functions(instance);

    VkDebugReportCallbackEXT callback = register_debug_callback(instance);

    VkDevice device    = lu_create_device(instance);

}

void lu_free_vulkan() {
    // TODO:
    //
    // TODO: def
    // DestroyDebugUtilMessengerEXT(instance, callback, NULL);
    // vkDestroyInstance(instance, NULL);
}
