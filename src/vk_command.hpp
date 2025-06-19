#pragma once

#include "common.hpp"
#include "scene/scene_obj.hpp"
#include "vk_device.hpp"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>


namespace HelloTriangle {
    namespace command_objects {
        class CommandPool {
            public:
            CommandPool(Device& device);
            ~CommandPool();
            
            VkCommandPool& get();
            private:
            VkCommandPool _command_pool;
            Device& _device;
        };
        class CommandBuffer {
            public: 
            CommandBuffer(Device& device, CommandPool& CommandPool, VkCommandBufferLevel level);
            ~CommandBuffer();
            void create(Device& device, CommandPool& CommandPool, VkCommandBufferLevel level);
            void begin(VkCommandBufferUsageFlags usage);
            void end();
            void submit();
            void reset();
            //secondary coming soon
            VkCommandBuffer& get() {};
            private:
            VkCommandBuffer _command_buffer;
            
            Device& _device;
            //CommandPool& _command_pool;
        };
    }
    //command wrappers for VkCmd*s coming soom
}
