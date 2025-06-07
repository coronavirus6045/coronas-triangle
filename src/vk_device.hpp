#pragma once

#include "common.hpp"
#include <vulkan/vulkan_core.h>


namespace HelloTriangle {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };
    //Device holds both vk physical device and device.

    class Device {
        public:
            Device();
            ~Device();
            void query_physical_device();
            void create_device();
            VkPhysicalDeviceProperties& get_device_properties();
            VkPhysicalDevice& get_physical_device();
            VkDevice& get_device() {return _device;}
            VkQueue& get_graphics_queue() {return _graphics_queue;}
            const uint32_t get_queue_family() {return _graphics_family.has_value() ? _graphics_family.value() : 0;}
        private:
        VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
        VkDevice _device;
        VkPhysicalDeviceProperties _device_properties;
        VkPhysicalDeviceFeatures2 _device_features;
        VkPhysicalDeviceVulkan11Features _device_features_11;
        VkPhysicalDeviceVulkan12Features _device_features_12;
        VkQueue _graphics_queue;
        std::optional<uint32_t> _graphics_family;
    };

    std::string get_device_name();
    class create_device {
        public:
        create_device();
        ~create_device();
        void find_physical_device(VkInstance instance, VkSurfaceKHR surface);
        void create_logical_device(VkSurfaceKHR surface);
        VkPhysicalDevice& get_physical_device() {return physicalDevice;;}
        VkDevice& get_device() {return device;}
        VkQueue& get_graphics_queue() {return graphicsQueue;}
        VkQueue& get_present_queue() {return presentQueue;}
        static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
       

        private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    };
}
