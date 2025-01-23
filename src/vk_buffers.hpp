#pragma once

#include "common.hpp"
#include "scene/scene_obj.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace HelloTriangle {
    class buffer_creation {
        public:
        buffer_creation(VkDevice& device_arg, VkPhysicalDevice& physical_device);
        ~buffer_creation();
        void create_vertex_buffer(const std::vector<vertex> vertices);
        VkBuffer& get_vertex_buffer() {return vertexBuffer;}
        private:
        VkBuffer vertexBuffer;
        VkMemoryRequirements memRequirements;
        VkPhysicalDeviceMemoryProperties memProperties;
        VkDeviceMemory vertexBufferMemory;
        VkDevice& device;
        VkPhysicalDevice& physicalDevice;
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    };
}