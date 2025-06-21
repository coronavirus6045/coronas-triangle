#include "vk_buffers.hpp"
#include "common.hpp"
#include "vk_device.hpp"
#include <cstdint>
#include <vector>
using HelloTriangle::Buffer;
using HelloTriangle::Device;
//buffer_creation::buffer_creation(Device& device_arg, VkPhysicalDevice& physical_device, VkQueue& graphics_queue, VkCommandPool& command_pool) : device(device_arg), physicalDevice(physical_device), commandPool(command_pool), graphicsQueue(graphics_queue) {}

Buffer::Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) : _device(device) {
    create(device, size, usage, properties);
}
void Buffer::create(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    CHECK_FOR_VK_RESULT(vkCreateBuffer(_device.get_device(), &bufferInfo, nullptr, &_buffer), "")

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(_device.get_device(), _buffer, &memory_requirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memory_requirements.size;
    allocInfo.memoryTypeIndex = find_memory_type(_device, memory_requirements.memoryTypeBits, properties);
    //how 2 be inconsistent: lambdas
    //tiny helpers that are NOT used by other classes.
    CHECK_FOR_VK_RESULT(vkAllocateMemory(_device.get_device(), &allocInfo, nullptr, &_buffer_memory), "")

    vkBindBufferMemory(_device.get_device(), _buffer, _buffer_memory, 0);
    _buffer_size = size;
    _device = device;
}

void Buffer::map_memory(void* data, uint64_t size, uint64_t offset, uint32_t flags = 0) {
    vkMapMemory(_device.get_device(), _buffer_memory, 0, size, 0, &_buffer_map);
    memcpy(_buffer_map, data, (size_t) size);
}

void Buffer::unmap_memory() {
    vkUnmapMemory(_device.get_device(), _buffer_memory);
}

void Buffer::update_memory_map(void* data) {
    memcpy(_buffer_map, data, (size_t) size);
}

Buffer create_vertex_buffer(Device& device, VkCommandPool& command_pool, const std::vector<vertex> vertices) {
    VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

    Buffer staging_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    staging_buffer.map_memory((void*) vertices.data(), buffer_size, 0);
    staging_buffer.unmap_memory();

    Buffer vertex_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
 
    copy_buffer(device, staging_buffer, vertex_buffer, command_pool, device.get_graphics_queue());

    //Staging buffer will get destroyed by destructor.
    return vertex_buffer;
}

Buffer create_index_buffer(Device& device, VkCommandPool& command_pool, const std::vector<uint32_t> indices) {
    VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();
    
    Buffer staging_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    staging_buffer.map_memory((void*) indices.data(), buffer_size, 0);

    Buffer index_buffer(device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    copy_buffer(device, staging_buffer, index_buffer, command_pool, device.get_graphics_queue());

    //Ditto with function above
    return index_buffer;
}
std::vector<Buffer> create_uniform_buffers(Device& device) {
    VkDeviceSize buffer_size = sizeof(HelloTriangle::uniform_Buffer);
    std::vector<Buffer> uniform_buffers(MAX_FRAMES_IN_FLIGHT);
    uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniform_buffers.emplace_back(Buffer(device, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
        uniform_buffers[i].map_memory()
    }
}
void copy_buffer(Device& device, Buffer& src_buffer, Buffer& dst_buffer, CommandPool command_pool, VkQueue queue) {
    VkCommandBufferAllocateInfo allocinfo{};
    allocinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocinfo.commandPool = command_pool.get();
    allocinfo.commandBufferCount = 1;

    command_objects::CommandBuffer copy_command_buffer(device, command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

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
    copy_command_buffer.submit();
}

uint32_t find_memory_type(Device& device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(device.get_physical_device(), &memory_properties);

    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
        if (typeFilter & (i << i) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
}

//Move to main
void buffer_creation::update_uniform_buffer(uint32_t current_frame, VkExtent2D extent) {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto current_time = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

    HelloTriangle::uniform_Buffer ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float) extent.height, 0.0f, 10.0f);
    ubo.proj[1][1] *= -1;
    memcpy(uniformBufferMaps[current_frame], &ubo, sizeof(ubo));
}
Buffer::~Buffer() {
    vkDestroyBuffer(_device.device, _buffer, nullptr);
    vkFreeMemory(_device.device, _buffer_memory, nullptr);
}

