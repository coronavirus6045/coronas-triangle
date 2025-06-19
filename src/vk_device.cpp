#include "vk_device.hpp"
#include "vk_presentation.hpp"
#include "vk_debugmsg.hpp"
#include <vulkan/vulkan_core.h>

using HelloTriangle::Device;
using HelloTriangle::QueueFamilyIndices;



void Device::query_physical_device(VkInstance instance) {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    if (device_count == 0) {
        THROW_RUNTIME_ERROR("No Supported GPU? (megamind peeking)");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());


    for (const auto& i : devices) {
        if (isDeviceSuitable(i, surface)) {
            _physical_device = i;
            break;
        }
    }
     if (_physical_device == VK_NULL_HANDLE) {
         THROW_RUNTIME_ERROR("Get a new GPU bro, it aint gonna be suitable.")
    }
}

void Device::create_device(VkSurfaceKHR surface) {
    //QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &queue_family_count, queue_families.data());

    VkBool32 present_support;

    for (int i = 0; i < queue_family_count; i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            _graphics_family = i;
            break;
        }
    }

    vkGetPhysicalDeviceSurfaceSupportKHR(_physical_device, _graphics_family.value(), surface, &present_support);

    if (!present_support) {

    }

    std::vector<VkDeviceQueueCreateInfo> queue_infos;
    std::vector<uint32_t> family_indices = {_graphics_family.value()};
    //float queuePriority = 1.0f;

    for (uint32_t i : family_indices) {
        VkDeviceQueueCreateInfo queue_info{};
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = i;
        queue_info.queueCount = family_indices.size();
        //queue_info.pQueuePriorities = &queuePriority;
        queue_infos.push_back(queue_info);
    }

    std::vector<const char*> required_device_extensions = {}; //this screwed my clangd

    uint32_t extension_count = 0;
    vkEnumerateDeviceExtensionProperties(_physical_device, nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> device_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(_physical_device, nullptr, &extension_count, device_extensions.data());

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

    _device_features.pNext = &_device_features_11;
    _device_features_11.pNext = &_device_features_12;

    vkGetPhysicalDeviceFeatures2(_physical_device, &required_device_features);

    required_device_features_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
    required_device_features_11.pNext = &required_device_features_12;

    required_device_features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;

    required_device_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    required_device_features.pNext = &required_device_features_11;

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
    device_info.pNext = &required_device_features;
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
}

bool create_device::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices = findQueueFamilies(device, surface);
    bool extensionsSupported = checkDeviceExtensionSupport(device);
             
    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapchainSupport = presentation_setup::query_swapchain_support(device, surface);
        swapChainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
    }
    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}


bool create_device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();
}

QueueFamilyIndices create_device::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;                
            }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
                   break;
        }
            
        i++;
    }

    return indices;
}

create_device::~create_device() {
    vkDestroyDevice(device, nullptr);
}


//Ballsack Gaming
