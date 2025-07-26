#include "vk_descriptor.hpp"
#include "common.hpp"
#include <cstdint>
#include <glm/common.hpp>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

using HelloTriangle::DescriptorPool;
using HelloTriangle::DescriptorLayout;
using HelloTriangle::DescriptorSet;


//Descriptors are for shaders to access resources
//Descriptor layout
DescriptorLayout::DescriptorLayout() {
    //create(device);
}

DescriptorLayout::DescriptorLayout(const DescriptorLayout& layout) {
    *this = layout;
}

DescriptorLayout::DescriptorLayout(DescriptorLayout&& layout) noexcept {
    *this = std::move(layout);
}

DescriptorLayout& DescriptorLayout::operator=(const DescriptorLayout& layout) {
    std::cout << "dl copy\n";
    _descriptor_layout = layout._descriptor_layout;
    //_bindings = layout._bindings;
    _device = layout._device;
    return *this;
}

DescriptorLayout& DescriptorLayout::operator=(DescriptorLayout&& layout) noexcept {
    std::cout << "dl move\n";
    _descriptor_layout = layout._descriptor_layout;
    //_bindings = layout._bindings;
    _device = layout._device;

    layout._descriptor_layout = nullptr;
    layout._device = nullptr;
    return *this;
}


void DescriptorLayout::create(Device& device, std::vector<DescriptorBinding> bindings) {
    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    std::vector<VkDescriptorSetLayoutBinding> _bindings;

    _bindings.resize(bindings.size());

    for (uint32_t i = 0; i < bindings.size(); i++) {
        _bindings[i].binding = bindings[i].binding;
        _bindings[i].descriptorType = bindings[i].descriptor_type;
        _bindings[i].descriptorCount = bindings[i].descriptor_count;
        _bindings[i].stageFlags = bindings[i].stage_flags;
        _bindings[i].pImmutableSamplers = nullptr;
    }

    layout_info.bindingCount = (uint32_t) _bindings.size();
    layout_info.pBindings = _bindings.data();


    CHECK_FOR_VK_RESULT(vkCreateDescriptorSetLayout(device.get_device(), &layout_info, nullptr, &_descriptor_layout), "")
    _device = &device;
}

/*
void DescriptorLayout::add_binding(uint32_t binding, uint32_t descriptor_count, VkDescriptorType type, VkShaderStageFlags stage_flags) {
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding;
    layout_binding.descriptorType = type;
    layout_binding.descriptorCount = descriptor_count;
    layout_binding.stageFlags = stage_flags;
    layout_binding.pImmutableSamplers = nullptr;

    _bindings.push_back(layout_binding);
}
*/
DescriptorLayout::~DescriptorLayout() {
    std::cout << "WTF\n";
    vkDestroyDescriptorSetLayout(_device->get_device(), _descriptor_layout, nullptr);
}

DescriptorPool::DescriptorPool(Device& device, uint32_t max_sets, std::vector<DescriptorBinding> bindings) {
    create(device, max_sets, bindings);
}

DescriptorPool::DescriptorPool(const DescriptorPool& pool) {
    *this = pool;
}

DescriptorPool::DescriptorPool(DescriptorPool&& pool) noexcept {
    *this = std::move(pool);
}

DescriptorPool& DescriptorPool::operator=(const DescriptorPool& pool) {
    _descriptor_pool = pool._descriptor_pool;
    _device = pool._device;
    return *this;
}

DescriptorPool& DescriptorPool::operator=(DescriptorPool&& layout) noexcept {
    _descriptor_pool = layout._descriptor_pool;
    _device = layout._device;

    layout._descriptor_pool = nullptr;
    layout._device = nullptr;
    return *this;
}

void DescriptorPool::create(Device& device, uint32_t max_sets, std::vector<DescriptorBinding> bindings) {
   // VkDescriptorPoolSize pool_size{};
    // need to fix this
    std::vector<VkDescriptorPoolSize> sizes;
    sizes.resize(bindings.size());
    for (uint32_t i = 0; i < bindings.size(); i++) {
        sizes[i].descriptorCount = (bindings[i].descriptor_count * max_sets);
        sizes[i].type = bindings[i].descriptor_type;
    }
    //pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //pool_size.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = sizes.size();
    pool_info.pPoolSizes = sizes.data();
    pool_info.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    CHECK_FOR_VK_RESULT(vkCreateDescriptorPool(device.get_device(), &pool_info, nullptr, &_descriptor_pool), "")
    _device = &device;
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(_device->get_device(), _descriptor_pool, nullptr);
}

DescriptorSet::DescriptorSet() {}

void DescriptorSet::allocate(Device& device, DescriptorPool& pool, DescriptorLayout& layout) {
    //std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool.get();
    alloc_info.descriptorSetCount = 1;
    VkDescriptorSetLayout* ly = &layout.get_layout();
    alloc_info.pSetLayouts = ly;
    //descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    //CHECK_FOR_VK_RESULT(vkAllocateDescriptorSets(device.get_device(), &alloc_info, &_descriptor_set), "")
    VkResult res = vkAllocateDescriptorSets(device.get_device(), &alloc_info, &_descriptor_set);
    _device = &device;
}


// Struct time!
void DescriptorSet::write_descriptor(DescriptorBinding binding, Buffer* buffer, uint32_t offset, uint64_t range, Image* image){
    //for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

    VkWriteDescriptorSet descriptor_write{};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = _descriptor_set;
    descriptor_write.dstBinding = binding.binding;
    descriptor_write.dstArrayElement = 0;

    descriptor_write.descriptorType = binding.descriptor_type;
    descriptor_write.descriptorCount = 1;
    //descriptor_write.pTexelBufferView = nullptr;

    if (buffer && range > 0 && image == nullptr) {
        VkDescriptorBufferInfo buffer_info{};
        buffer_info.buffer = buffer->buffer();

        buffer_info.offset = 0;
        buffer_info.range = (VkDeviceSize) range;

        descriptor_write.pBufferInfo = &buffer_info;
    } else if (image && buffer == nullptr) {
        VkDescriptorImageInfo image_info{};
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = image->get_image_view();
        image_info.sampler = image->get_sampler();

        descriptor_write.pImageInfo = &image_info;
    } else {
        THROW_RUNTIME_ERROR("Invalid usage of write_descriptor()");
    }

    vkUpdateDescriptorSets(_device->get_device(), 1, &descriptor_write, 0, nullptr);

}

DescriptorSet::~DescriptorSet() {}
