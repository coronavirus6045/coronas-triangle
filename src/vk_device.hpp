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

    class create_device {
        public:
        void find_physical_device(VkInstance instance);
        void create_logical_device(VkSurfaceKHR surface);
        VkDevice& get_device() {return device;}
        VkQueue& get_graphics_queue() {return graphicsQueue;}
        VkQueue& get_present_queue() {return presentQueue;}
        private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        bool isDeviceSuitable(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    };
}