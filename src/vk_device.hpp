#pragma once

#include "common.hpp"


namespace HelloTriangle {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

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