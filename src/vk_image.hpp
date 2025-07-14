#pragma once

#include "common.hpp"
#include "vk_buffers.hpp"
#include "vk_command.hpp"
#include "vk_device.hpp"
#include <vulkan/vulkan_core.h>

namespace HelloTriangle {
class Image {
    public:
        //Initialize without loading image file and/or creating VkImage
        //Load image file and create its VkImage
        Image();
        Image(Device& device,
              uint32_t width,
              uint32_t height,
              VkFormat format,
              VkImageTiling tiling,
              VkFilter filter,
              VkSamplerAddressMode address_mode,
              float anisotropy_level,
              VkImageUsageFlags usage,
              VmaMemoryUsage memory_usage);
        ~Image();
        //Buffer(const Buffer& buffer);
        Image(Image&& image) noexcept;
        //Buffer& operator=(const Buffer& buffer);
        Image& operator=(Image&& image) noexcept;

        //creates VkImage (for fine grained stuff)
        void create(Device& device,
                    uint32_t width,
                    uint32_t height,
                    VkFormat format,
                    VkImageTiling tiling,
                    VkFilter filter,
                    VkSamplerAddressMode address_mode,
                    float anisotropy_level,
                    VkImageUsageFlags usage,
                    VmaMemoryUsage memory_usage);
        //image and its memory
        void transition_image_layout(CommandPool& command_pool,
                                     VkFormat format,
                                     VkImageLayout new_layout);
        void copy_buffer_to_image(CommandPool& command_pool,
                                  Buffer& buffer,
                                  uint32_t width,
                                  uint32_t height);
        VkImage& get_image() { return _image; }
        VkSampler get_sampler() { return _sampler; }
        //VkDeviceMemory& get_image_memory() {return _image_memory;}
        VkImageView& get_image_view() { return _image_view; }
        uint32_t get_image_width() { return _image_width; }
        uint32_t get_image_height() { return _image_height; }

    private:
        VkImage _image;
        VmaAllocation _allocation;
        //VkDeviceMemory _image_memory;
        VkImageView _image_view;
        VkSampler _sampler;
        uint32_t _image_width;
        uint32_t _image_height;
        VkImageLayout _layout;
        //int32_t _image_channels;

        Device* _device;
};
//Image load_image_from_file(std::string& path);

//void copy_buffer_to_image(Buffer& buffer, Image& image, uint32_t width, uint32_t height);

}
