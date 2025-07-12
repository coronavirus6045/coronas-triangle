#include "vk_device.hpp"
#include "vk_debugmsg.hpp"
#include <vulkan/vulkan_core.h>

using HelloTriangle::Device;

// i should put this to create()
Device::Device(Instance& instance, uint32_t gpu_index, VkSurfaceKHR surface) {
    create(instance, gpu_index, surface);
}


void Device::create(Instance& instance, uint32_t gpu_index, VkSurfaceKHR surface) {
    // Physical device setup

    uint32_t physical_device_count = 0;
    vkEnumeratePhysicalDevices(instance.get(), &physical_device_count, nullptr);

    if (physical_device_count == 0) {
        THROW_RUNTIME_ERROR("No Supported GPU? (megamind peeking)");
    }

    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    vkEnumeratePhysicalDevices(instance.get(), &physical_device_count, physical_devices.data());

    for (uint32_t i = 0; i < physical_devices.size(); i++) {
        if (i == gpu_index) {
            _physical_device = physical_devices[i];
            break;
        } else {
            THROW_RUNTIME_ERROR("Invalid GPU specified in index.");
        }
    }

    //QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(_physical_device,
                                             &queue_family_count,
                                             queue_families.data());

    VkBool32 present_support;

    for (uint32_t i = 0; i < queue_family_count; i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            _graphics_family = i;
            break;
        }
    }

    vkGetPhysicalDeviceSurfaceSupportKHR(_physical_device,
                                         _graphics_family.value(),
                                         surface,
                                         &present_support);

    if (!present_support) {
        THROW_RUNTIME_ERROR("Physical device does not support present.");
    }

    // Logical device setup

    std::vector<VkDeviceQueueCreateInfo> queue_infos;
    std::vector<uint32_t> family_indices = {_graphics_family.value()};
    float queue_priority = 1.0f;

    for (uint32_t i : family_indices) {
        VkDeviceQueueCreateInfo queue_info{};
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = i;
        queue_info.queueCount = family_indices.size();
        queue_info.pQueuePriorities = &queue_priority;
        queue_infos.push_back(queue_info);
    }

    //this screwed my clangd so ill use its absolute name instead of its enum (e.g. VK_KHR_swapchain instead of VK_KHR_SWAPCHAIN_EXTENSION_NAME)
    std::vector<const char*> required_device_extensions = {"VK_KHR_swapchain"};

    uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties(_physical_device, nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> device_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(_physical_device,
                                         nullptr,
                                         &extension_count,
                                         device_extensions.data());

    for (const auto& i : required_device_extensions) {
        for (const auto& j : device_extensions) {
            if (strcmp(i, j.extensionName) == 0) {
                break;
            } else {
            }
        }
    }

    //VkPhysicalDeviceFeatures required_device_features_10 = {};
    VkPhysicalDeviceVulkan11Features required_device_features_11 = {};
    VkPhysicalDeviceVulkan12Features required_device_features_12 = {};
    VkPhysicalDeviceFeatures2 required_device_features{};

    //This is correct
    _device_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    //_device_features.pNext = &_device_features_11;
    _device_features_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
    //_device_features_11.pNext = &_device_features_12;
    _device_features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;

    vkGetPhysicalDeviceFeatures2(_physical_device, &_device_features);

    required_device_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    required_device_features.pNext = &required_device_features_11;

    required_device_features_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
    required_device_features_11.pNext = &required_device_features_12;

    required_device_features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;



//we aint doing all that so we did a macro yay (for 1.0 - 1.2)
#define DEVICE_FEATURE_10_ENABLE_IF(feature) \
    if (_device_features.feature) { \
        required_device_features.features.feature = _device_features_10.feature; \
    }

#define DEVICE_FEATURE_11_ENABLE_IF(feature) \
    if (_device_features.feature) { \
        required_device_features_11.feature = _device_features_11.feature; \
    }

#define DEVICE_FEATURE_12_ENABLE_IF(feature) \
    if (_device_features.feature) { \
        required_device_features_12.feature = _device_features_12.feature; \
    }

    // Device Features (features grouped by version, separated by newline)

    VkDeviceCreateInfo device_info{};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    //device_info.pNext = &required_device_features;
    device_info.pNext = &_device_features;
    device_info.queueCreateInfoCount = static_cast<uint32_t>(queue_infos.size());
    device_info.pQueueCreateInfos = queue_infos.data();
    device_info.pEnabledFeatures = nullptr;
    device_info.enabledExtensionCount = (uint32_t) required_device_extensions.size();
    device_info.ppEnabledExtensionNames = required_device_extensions.data();

    if (enableValidationLayers) {
        device_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        device_info.ppEnabledLayerNames = validationLayers.data();
    } else {
        device_info.enabledLayerCount = 0;
    }
    CHECK_FOR_VK_RESULT(vkCreateDevice(_physical_device, &device_info, nullptr, &_device), "")
    vkGetDeviceQueue(_device, _graphics_family.value(), 0, &_graphics_queue);
    //vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &presentQueue);
    volkLoadDevice(_device);

    //Configure VMA allocator

    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.instance = instance.get();
    allocator_info.physicalDevice = _physical_device;
    allocator_info.device = _device;
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;
    allocator_info.flags = 0;

    VmaVulkanFunctions vma_functions{};
    vma_functions.vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) vkGetInstanceProcAddr;
    vma_functions.vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr) vkGetDeviceProcAddr;

    allocator_info.pVulkanFunctions = &vma_functions;

    vmaCreateAllocator(&allocator_info, &_allocator);
}

Device::~Device() {
    std::cout << "Goodbye....\n";
    vkDestroyDevice(_device, nullptr);
    vmaDestroyAllocator(_allocator);
}
