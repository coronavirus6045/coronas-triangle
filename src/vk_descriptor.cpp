#include "vk_descriptor.hpp"
#include "uniform_buffer_object.hpp"
#include "src/common.hpp"
#include <cstdint>
#include <glm/common.hpp>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

using HelloTriangle::DescriptorPool;
using HelloTriangle::DescriptorLayout;
using HelloTriangle::DescriptorSet;

descriptor_creation::descriptor_creation(VkDevice& device_arg) : device(device_arg) {

}

descriptor_creation::~descriptor_creation() {
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}
//Descriptors are for shaders to access resources
//Descriptor layout
HelloTriangle::DescriptorLayout::create() {
    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = (uint32_t) _bindings.size();
    layout_info.pBindings = _bindings.data();

    if (vkCreateDescriptorSetLayout(p_device.device, &layout_info, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("We cant make descriptor set layout");
    }
}

void DescriptorLayout::add_binding(uint32_t binding, uint32_t descriptor_count, VkDescriptorType type, VkShaderStageFlags stage_flags) {
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding;
    layout_binding.descriptorType = type;
    layout_binding.descriptorCount = descriptor_count;
    layout_binding.stageFlags = stage_flags;
    layout_binding.pImmutableSamplers = nullptr;

    _bindings.push_back(layout_binding);
}

void DescriptorPool::DescriptorPool() {
    VkDescriptorPoolSize pool_size{};
    pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_size.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    pool_info.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("DesPool error");
    }
}

void DescriptorSet::allocate(HelloTriangle::DescriptorPool pool, DescriptorLayout layout) {
    //std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool.get();
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout.get_layout();
    //descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(device, &alloc_info, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("DesSets allocate error");
    }
}

void DescriptorSet::write_descriptor(uint32_t binding, VkDescriptorType descriptor_type, buffer_creation::Buffer* buffer, uint32_t offset, VkDeviceSize range){
    //for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

    VkWriteDescriptorSet descriptor_write{};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = _descriptor_set;
    descriptor_write.dstBinding = binding;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = descriptor_type;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo = &buffer_info;
    descriptor_write.pImageInfo = nullptr;
    //descriptor_write.pTexelBufferView = nullptr;

    if (buffer != nullptr && range > 0) {
        VkDescriptorBufferInfo buffer_info{};
        buffer_info.buffer = buffer->buffer();
        buffer_info.offset = 0;
        buffer_info.range = ;

        descriptor_write.pBufferInfo = &buffer_info;
    } else if (image != nullptr) {
        VkDescriptorImageInfo image_info{};
        image_info.imageLayout = ;
        image_info.imageView = ;
    }

        vkUpdateDescriptorSets(_device.device, 1, &descriptor_write, 0, nullptr);

    //}
}
