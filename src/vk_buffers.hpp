#pragma once

#include "common.hpp"
#include "scene/scene_obj.hpp"
#include "uniform_buffer_object.hpp"
#include "vk_command.hpp"
#include "vk_device.hpp"
#include <chrono>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace HelloTriangle {
// We'll use VMA later
class Buffer {
    public:
        Buffer();
        Buffer(const Buffer& buffer);
        Buffer(Buffer&& buffer) noexcept;
        Buffer& operator=(const Buffer& buffer);
        Buffer& operator=(Buffer&& buffer) noexcept;
        Buffer(Device& device,
               VkDeviceSize size,
               VkBufferUsageFlags usage,
               VkMemoryPropertyFlags properties);
        ~Buffer();

        void create(Device& device,
                    VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties);

        const VkBuffer& buffer() const { return _buffer; }
        const VkDeviceMemory& buffer_memory() const { return _buffer_memory; }
        const VkDeviceSize& buffer_size() const { return _buffer_size; }
        void map_memory(void* data, uint64_t size, uint64_t offset, uint32_t flags = 0);
        void unmap_memory();
        void update_memory_map(void* data, uint64_t size);

    private:
        void* _buffer_map;
        bool _is_copied;
        uint64_t _map_size;
        VkBuffer _buffer;
        VkDeviceMemory _buffer_memory;
        VkDeviceSize _buffer_size;

        Device* _device;
};
//Wrappers for different buffers but you can manually do it.
Buffer create_vertex_buffer(Device& device,
                            CommandPool& command_pool,
                            const std::vector<vertex> vertices);

Buffer create_index_buffer(Device& device,
                           CommandPool& command_pool,
                           const std::vector<uint32_t> indices);

Buffer create_uniform_buffer(Device& device, void* data, uint64_t size);

void copy_buffer(Device& device, Buffer& src_buffer, Buffer& dst_buffer, CommandPool& command_pool);

uint32_t find_memory_type(Device& device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
} // namespace HelloTriangle
