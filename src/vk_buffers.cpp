#include "vk_buffers.hpp"
#include "common.hpp"
#include "vk_device.hpp"
#include <cstdint>
#include <vector>
using HelloTriangle::Buffer;
using HelloTriangle::CommandPool;
using HelloTriangle::CommandBuffer;
using HelloTriangle::Device;
using HelloTriangle::vertex;
//buffer_creation::buffer_creation(Device& device_arg, VkPhysicalDevice& physical_device, VkQueue& graphics_queue, VkCommandPool& command_pool) : device(device_arg), physicalDevice(physical_device), commandPool(command_pool), graphicsQueue(graphics_queue) {}

Buffer::Buffer(){}

//Buffer::Buffer(const Buffer& buffer) {}

Buffer::Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage) {
    create(device, size, usage, memory_usage);
    //_is_copied = false;
}

Buffer::Buffer(const Buffer& buffer) {
    std::cout << "BUFFER COPY\n";
    *this = buffer;
}

Buffer::Buffer(Buffer&& buffer) noexcept {
    *this = std::move(buffer);
}

Buffer& Buffer::operator=(const Buffer& buffer) {
    std::cout << "BUFFER COPY\n";
    _buffer = buffer._buffer;
    _allocation = buffer._allocation;
    _buffer_size = buffer._buffer_size;

    //shallow copy again sinc pointer shit
    _buffer_map = buffer._buffer_map;
    //memcpy(buffer._buffer_map, _buffer_map, _map_size);

    _map_size = buffer._map_size;
    _device = buffer._device;
    //_is_copied = true;
    return *this;
}

Buffer& Buffer::operator=(Buffer&& buffer) noexcept {
    std::cout << "BUFFER MOVE\n";

    _buffer = buffer._buffer;
    _allocation = buffer._allocation;
    _buffer_size = buffer._buffer_size;

    _buffer_map = buffer._buffer_map;
    _map_size = buffer._map_size;
    _device = buffer._device;

    buffer._buffer = nullptr;
    buffer._allocation = nullptr;
    buffer._map_size = 0;
    buffer._buffer_size = 0;
    buffer._buffer_map = nullptr;
    buffer._device = nullptr;
    //_is_copied = true;

    return *this;
}



void Buffer::create(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage) {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VmaAllocationCreateInfo allocation_info{};
    allocation_info.usage = memory_usage;

    if (memory_usage == VMA_MEMORY_USAGE_AUTO_PREFER_HOST) {
        // Checks if its staging buffer (CPU -> GPU)
        if (usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT || usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
            allocation_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            allocation_info.preferredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        }
        // Readback (GPU -> CPU)
        if (usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT) {
            allocation_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
            allocation_info.preferredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        }
        allocation_info.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    } else if (memory_usage == VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE) {
        // GPU only buffer
        allocation_info.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }

    //CHECK_FOR_VK_RESULT(vkAllocateMemory(device.get_device(), &allocInfo, nullptr, &_buffer_memory), "")

    //VkResult res = (vmaCreateBuffer(device.get_allocator(), &buffer_info, &allocation_info, &_buffer, &_allocation, nullptr));
    CHECK_FOR_VK_RESULT(vmaCreateBuffer(device.get_allocator(), &buffer_info, &allocation_info, &_buffer, &_allocation, nullptr), "")

    //vkBindBufferMemory(device.get_device(), _buffer, _buffer_memory, 0);
    _buffer_size = size;
    _device = &device;
    //_is_copied = false;
}

void Buffer::map_memory(void* data, uint64_t size, uint64_t offset, uint32_t flags) {
    assert(size <= _buffer_size);
    vmaMapMemory(_device->get_allocator(), _allocation, &_buffer_map);
    memcpy(_buffer_map, data, size);
    _map_size = size;
    _is_mapped = true;
}

void Buffer::unmap_memory() {
    vmaUnmapMemory(_device->get_allocator(), _allocation);
    _is_mapped = false;
}

void Buffer::update_memory_map(void* data, uint64_t size) {
    assert(size <= _map_size);
    memcpy(_buffer_map, data, (size_t) size);
}

Buffer HelloTriangle::create_vertex_buffer(Device& device, CommandPool& command_pool, const std::vector<vertex> vertices) {
    VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

    Buffer staging_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
    staging_buffer.map_memory((void*) vertices.data(), buffer_size, 0);
    staging_buffer.unmap_memory();

    Buffer vertex_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,  VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
 
    copy_buffer(device, staging_buffer, vertex_buffer, command_pool);

    //Staging buffer will get destroyed by destructor.
    return vertex_buffer;
}

//these things should be in a wrappers file
Buffer HelloTriangle::create_index_buffer(Device& device, CommandPool& command_pool, const std::vector<uint32_t> indices) {
    VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();
    
    Buffer staging_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
    staging_buffer.map_memory((void*) indices.data(), buffer_size, 0);

    Buffer index_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,  VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
    copy_buffer(device, staging_buffer, index_buffer, command_pool);

    //Ditto with function above
    return index_buffer;
}
Buffer HelloTriangle::create_uniform_buffer(Device& device, void* data, uint64_t size) {
    Buffer uniform_buffer(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
    uniform_buffer.map_memory(data, size, 0, 0);
    return uniform_buffer;
}
void HelloTriangle::copy_buffer(Device& device, Buffer& src_buffer, Buffer& dst_buffer, CommandPool& command_pool) {
    CommandBuffer copy_command_buffer(device, command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    //VkCommandBufferBeginInfo begin_info{};
    //begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    copy_command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = src_buffer.buffer_size();
    vkCmdCopyBuffer(copy_command_buffer.get(), src_buffer.buffer(), dst_buffer.buffer(), 1, &copy_region);
    //vkEndCommandBuffer(copy_command_buffer.get());
    copy_command_buffer.end();
    copy_command_buffer.submit(nullptr, nullptr, nullptr, nullptr);
}
/*
uint32_t HelloTriangle::find_memory_type(Device& device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(device.get_physical_device(), &memory_properties);

    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
        if (typeFilter & (i << i) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
}
*/

Buffer::~Buffer() {
    std::cout << "BUFFER OOF\n";
    //_is_copied = false;
    //if (_is_copied == true) {
    //    _buffer = nullptr;
    //    _allocation = nullptr;
    //}
        // at exit for some reason when destructor is called without this is copied if block it segfaults
    if (_is_mapped == true) {
        unmap_memory();
    }
    if (_buffer != nullptr) {
        vmaDestroyBuffer(_device->get_allocator(), _buffer, _allocation);
        //_buffer = nullptr;
        _is_oofed = true;
    } else {
        std::cout << "copied buffer is destroyed\n";
    }
}

