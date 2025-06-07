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
    void create(void* p_next);
    void add_binding(uint32_t binding, VkShaderStageFlags stage_flags);
    VkDescriptorSetLayout& get_layout() {return _descriptor_layout;}
    private:
    VkDescriptorSetLayout _descriptor_layout;
    std::vector<VkDescriptorSetLayoutBinding> _bindings;

    Device& p_device;
    };
    //Pool will have set creation.
    class DescriptorPool {
        public:
    DescriptorPool(Device& device, uint32_t max_sets, VkDescriptorType descriptor_type);
    ~DescriptorPool();
    void create();
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

    void allocate();
    //
    void write_descriptor(uint32_t binding, VkDescriptorType descriptor_type, buffer_creation::Buffer* buffer, ImageTexture* image, VkImageView image_view,);
    VkDescriptorSet& get();
    private:
    VkDescriptorSet _descriptor_set;
        Device& _device;
    };


    class descriptor_creation {
        public:
        descriptor_creation(VkDevice& device_arg);
        ~descriptor_creation();
        void create_descriptor_set_layout();
        void create_descriptor_pool();
        void create_descriptor_sets(const std::vector<VkBuffer>& uniform_buffers);
        VkDescriptorSetLayout& get_descriptor_set_layout(){return descriptorSetLayout;}
        VkDescriptorPool get_descriptor_pool(){return descriptorPool;}
        std::vector<VkDescriptorSet>& get_descriptor_sets(){return descriptorSets;}
        private:
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;
        VkDevice& device;
    };
}
