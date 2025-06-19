#pragma once

#include "common.hpp"
#include "vk_buffers.hpp"
#include "vk_device.hpp"
#include <vulkan/vulkan_core.h>

namespace HelloTriangle {
    class DescriptorLayout {
    public:
    DescriptorLayout(Device& device);
    ~DescriptorLayout();
    void create(Device& device);
    void add_binding(uint32_t binding, uint32_t descriptor_count, VkDescriptorType type, VkShaderStageFlags stage_flags);
    VkDescriptorSetLayout& get_layout() {return _descriptor_layout;}
    private:
    VkDescriptorSetLayout _descriptor_layout;
    std::vector<VkDescriptorSetLayoutBinding> _bindings;

    Device& _device;
    };
    //Pool will have set creation.
    class DescriptorPool {
    public:
    DescriptorPool(Device& device, uint32_t max_sets, VkDescriptorType descriptor_type);
    ~DescriptorPool();
    void create(Device& device, uint32_t max_sets, VkDescriptorType descriptor_type);
    void reset();
    VkDescriptorPool& get();

    private:
    VkDescriptorPool _descriptor_pool;
    };
    //Constructor will allocate but you can reallocate

    class DescriptorSet {
    public:
    DescriptorSet();
    ~DescriptorSet();

    void allocate(Device& device, DescriptorPool pool, DescriptorLayout layout);
    //
    void write_descriptor(uint32_t binding, VkDescriptorType descriptor_type, Buffer* buffer, VkImageView image_view,);
    VkDescriptorSet& get();
    private:
    VkDescriptorSet _descriptor_set;
        Device& _device;
    };
}
