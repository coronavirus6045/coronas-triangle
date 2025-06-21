#pragma once

#include "common.hpp"
#include "scene/scene_obj.hpp"
#include "vk_device.hpp"
#include "vk_command.hpp"
#include <cstdint>
#include <vector>
#include "uniform_buffer_object.hpp"
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

namespace HelloTriangle {
    // We'll use VMA later5
        class Buffer { //buffer object wrapper so i can have both buffer and its memory in one object
            public:
            Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
            ~Buffer();
            void create(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
            //Buffer and its memory is always const since you do not modify it directly, use map_memory() and assign_memory() instead.
            //These can be used when using native vk functions.
            const VkBuffer& buffer() const {return _buffer;}
            const VkDeviceMemory& buffer_memory() const {return _buffer_memory;}
            const VkDeviceSize& buffer_size() const {return _buffer_size;}
            //void* data - anything like vertices, indices, etc. Needs to cast to void pointer
            void map_memory(void* data, uint64_t size, uint64_t offset, uint32_t flags = 0);
            void unmap_memory();
            void update_memory_map(void* data);

            private:
            void* _buffer_map;
            VkBuffer _buffer;
            VkDeviceMemory _buffer_memory;
            VkDeviceSize _buffer_size;
            
            Device& _device;
        };
        //Wrappers for different buffers but you can manually do it.
        Buffer create_vertex_buffer(Device& device, VkCommandPool& command_pool, const std::vector<vertex> vertices);
        Buffer create_index_buffer(Device& device, VkCommandPool& command_pool, const std::vector<uint32_t> indices);
        Buffer create_uniform_buffers(Device& device);
        void copy_buffer(Device& device, Buffer& src_buffer, Buffer& dst_buffer, VkCommandPool command_pool, VkQueue queue);
        uint32_t find_memory_type(Device& device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
}
