#pragma once

#include "common.hpp"
#include "vk_device.hpp"
#include "vk_syncobjects.hpp"

#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>


namespace HelloTriangle {
        class CommandPool {
            public:
            CommandPool();
            CommandPool(Device& device);
            ~CommandPool();
            void create(Device& device);
            VkCommandPool& get() {return _command_pool;}
            private:
            VkCommandPool _command_pool;
            Device* _device;
        };
        class CommandBuffer {
            public:
            CommandBuffer();
            CommandBuffer(Device& device, CommandPool& command_pool, VkCommandBufferLevel level);
            void create(Device& device, CommandPool& command_pool, VkCommandBufferLevel level);
            void begin(VkCommandBufferUsageFlags usage);
            void end();
            void submit(std::vector<Semaphore>* wait, std::vector<Semaphore>* signal, Fence fence);
            void reset();
            //secondary coming soon
            VkCommandBuffer& get() {return _command_buffer;}
            private:
            VkCommandBuffer _command_buffer;
            
            Device* _device;
            //CommandPool& _command_pool;
        };

    //command wrappers for VkCmd*s coming soom
}
