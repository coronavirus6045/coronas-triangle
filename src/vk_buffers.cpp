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

Buffer::Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    create(device, size, usage, properties);
    _is_copied = false;
}

Buffer::Buffer(const Buffer& buffer) {
    *this = buffer;
}

Buffer::Buffer(Buffer&& buffer) noexcept {
    *this = std::move(buffer);
}

Buffer& Buffer::operator=(const Buffer& buffer) {
    std::cout << "BUFFER COPY\n";
    _buffer = buffer._buffer;
    _buffer_memory = buffer._buffer_memory;
    _buffer_size = buffer._buffer_size;

    memcpy(buffer._buffer_map, _buffer_map, _map_size);

    _map_size = buffer._map_size;
    _device = buffer._device;
    _is_copied = true;
    return *this;
}

Buffer& Buffer::operator=(Buffer&& buffer) noexcept {
    std::cout << "BUFFER MOVE\n";

    _buffer = buffer._buffer;
    _buffer_memory = buffer._buffer_memory;
    _buffer_size = buffer._buffer_size;

    _buffer_map = buffer._buffer_map;
    _map_size = buffer._map_size;
    _device = buffer._device;

    buffer._buffer = nullptr;
    buffer._buffer_memory = nullptr;
    buffer._buffer_map = nullptr;
    buffer._device = nullptr;

    return *this;
}

void Buffer::create(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    CHECK_FOR_VK_RESULT(vkCreateBuffer(device.get_device(), &bufferInfo, nullptr, &_buffer), "")

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(device.get_device(), _buffer, &memory_requirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memory_requirements.size;
    allocInfo.memoryTypeIndex = find_memory_type(device, memory_requirements.memoryTypeBits, properties);
    //how 2 be inconsistent: lambdas
    //tiny helpers that are NOT used by other classes.
    CHECK_FOR_VK_RESULT(vkAllocateMemory(device.get_device(), &allocInfo, nullptr, &_buffer_memory), "")

    vkBindBufferMemory(device.get_device(), _buffer, _buffer_memory, 0);
    _buffer_size = size;
    _device = &device;
}

void Buffer::map_memory(void* data, uint64_t size, uint64_t offset, uint32_t flags) {
    assert(size <= _buffer_size);
    vkMapMemory(_device->get_device(), _buffer_memory, 0, size, 0, &_buffer_map);
    memcpy(_buffer_map, data, (size_t) size);
    _map_size = size;
}

void Buffer::unmap_memory() {
    vkUnmapMemory(_device->get_device(), _buffer_memory);
}

void Buffer::update_memory_map(void* data, uint64_t size) {
    assert(size <= _map_size);
    memcpy(_buffer_map, data, (size_t) size);
}

Buffer HelloTriangle::create_vertex_buffer(Device& device, CommandPool& command_pool, const std::vector<vertex> vertices) {
    VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

    Buffer staging_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    staging_buffer.map_memory((void*) vertices.data(), buffer_size, 0);
    staging_buffer.unmap_memory();

    Buffer vertex_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
 
    copy_buffer(device, staging_buffer, vertex_buffer, command_pool);

    //Staging buffer will get destroyed by destructor.
    return vertex_buffer;
}

//these things should be in a wrappers file
Buffer HelloTriangle::create_index_buffer(Device& device, CommandPool& command_pool, const std::vector<uint32_t> indices) {
    VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();
    
    Buffer staging_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    staging_buffer.map_memory((void*) indices.data(), buffer_size, 0);

    Buffer index_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    copy_buffer(device, staging_buffer, index_buffer, command_pool);

    //Ditto with function above
    return index_buffer;
}
Buffer HelloTriangle::create_uniform_buffer(Device& device, void* data, uint64_t size) {
    Buffer uniform_buffer(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
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

uint32_t HelloTriangle::find_memory_type(Device& device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(device.get_physical_device(), &memory_properties);

    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
        if (typeFilter & (i << i) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
}


Buffer::~Buffer() {
    std::cout << "BUFFER OOF\n";
    //_is_copied = false;
    //if (_is_copied) {
    //    _buffer = nullptr;
    //    _buffer_memory = nullptr;
    //} else {
        // at exit for some reason when destructor is called without this is copied if block it segfaults
        vkDestroyBuffer(_device->get_device(), _buffer, nullptr);
        vkFreeMemory(_device->get_device(), _buffer_memory, nullptr);
    //}
}

