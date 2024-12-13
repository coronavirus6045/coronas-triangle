#pragma once

#include "common.hpp"


namespace HelloTriangle {
    class command_objects {
        public:
        command_objects(VkPhysicalDevice& physical_device, VkDevice& device_arg, VkSurfaceKHR& surface_arg);
        ~command_objects();
        void create_command_pool();
        void create_command_buffer();
        void record_command_buffer(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkExtent2D swapChainExtent, std::vector<VkFramebuffer> swapChainFramebuffers, uint32_t imageIndex, VkPipeline graphicsPipeline);
        
        std::vector<VkCommandBuffer>& get_command_buffers() {return commandBuffers;}
        private:
        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;

        VkPhysicalDevice& physicalDevice;
        VkDevice& device;
        VkSurfaceKHR& surface;
    };
}