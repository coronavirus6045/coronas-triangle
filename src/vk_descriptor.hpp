#pragma once

#include "common.hpp"
#include "vk_buffers.hpp"
#include "vk_device.hpp"
#include "vk_image.hpp"
#include <vulkan/vulkan_core.h>

//namespace ctBackend
namespace HelloTriangle {
struct DescriptorBinding {
        uint32_t binding;
        VkDescriptorType descriptor_type;
        uint32_t descriptor_count;
        VkShaderStageFlags stage_flags;
};

class DescriptorLayout {
    public:
        DescriptorLayout();
        //DescriptorLayout(Device& device);
        DescriptorLayout(const DescriptorLayout& layout);
        DescriptorLayout(DescriptorLayout&& layout) noexcept;
        DescriptorLayout& operator=(const DescriptorLayout& layout);
        DescriptorLayout& operator=(DescriptorLayout&& layout) noexcept;
        ~DescriptorLayout();

        void create(Device& device, std::vector<DescriptorBinding> bindings);
        /*void add_binding(uint32_t binding,
                         uint32_t descriptor_count,
                         VkDescriptorType type,
                         VkShaderStageFlags stage_flags);*/
        VkDescriptorSetLayout& get_layout() { return _descriptor_layout; }

    private:
        VkDescriptorSetLayout _descriptor_layout;
        //std::vector<VkDescriptorSetLayoutBinding> _bindings;

        Device* _device;
};
// Let's go, gets grow, lets GROW a GARDEN!
// whoever said that above, please EXECUTE him!
//Pool will have set creation.
class DescriptorPool {
    public:
        DescriptorPool();
        DescriptorPool(Device& device, uint32_t max_sets, std::vector<DescriptorBinding> bindings);
        DescriptorPool(const DescriptorPool& pool);
        DescriptorPool(DescriptorPool&& pool) noexcept;
        DescriptorPool& operator=(const DescriptorPool& pool);
        DescriptorPool& operator=(DescriptorPool&& pool) noexcept;
        ~DescriptorPool();
        void create(Device& device, uint32_t max_sets, std::vector<DescriptorBinding> bindings);
        void reset();
        VkDescriptorPool& get() { return _descriptor_pool; }

    private:
        VkDescriptorPool _descriptor_pool;
        Device* _device;
};
//Constructor will allocate but you can reallocate

class DescriptorSet {
    public:
        DescriptorSet();
        // constructor that calls allocate()
        ~DescriptorSet();

        void allocate(Device& device, DescriptorPool& pool, DescriptorLayout& layout);
        //
        void write_descriptor(DescriptorBinding binding,
                              Buffer* buffer,
                              uint32_t offset,
                              uint64_t range,
                              Image* image);
        VkDescriptorSet& get() { return _descriptor_set; }

    private:
        VkDescriptorSet _descriptor_set;
        Device* _device;
};
} // namespace HelloTriangle
