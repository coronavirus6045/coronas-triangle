#pragma once

#include "common.hpp"
#include "vk_instance.hpp"
#include <vulkan/vulkan_core.h>

namespace HelloTriangle {
//Device holds both vk physical device and device.

class Device {
    public:
        Device();
        Device(Instance& instance, uint32_t gpu_index, VkSurfaceKHR surface);
        ~Device();
        void create(Instance& instance, uint32_t gpu_index, VkSurfaceKHR surface);
        // VkPhysicalDeviceProperties& get_device_properties();
        VkPhysicalDevice& get_physical_device() { return _physical_device; }
        VkDevice& get_device() { return _device; }
        VkQueue& get_graphics_queue() { return _graphics_queue; }
        VmaAllocator& get_allocator() { return _allocator; }
        const uint32_t get_queue_family() {
            return _graphics_family.has_value() ? _graphics_family.value() : 0;
        }

    private:
        VkPhysicalDevice _physical_device;
        VkDevice _device;
        VmaAllocator _allocator;
        VkPhysicalDeviceProperties _device_properties;
        VkPhysicalDeviceFeatures2 _device_features;
        VkPhysicalDeviceVulkan11Features _device_features_11;
        VkPhysicalDeviceVulkan12Features _device_features_12;
        VkQueue _graphics_queue;
        std::optional<uint32_t> _graphics_family;
};

// std::string get_device_name();
} // namespace HelloTriangle
