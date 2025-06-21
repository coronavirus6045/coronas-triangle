#pragma once

#include "common.hpp"
#include "vk_device.hpp"
#include "vk_buffers.hpp"
#include "vk_command.hpp"
#include <vulkan/vulkan_core.h>

namespace HelloTriangle {
        class Image {
            public:
            //Initialize without loading image file and/or creating VkImage
            //Load image file and create its VkImage
            Image();
            ~Image();
            //void load_image_file(std::string& path);
            //creates VkImage (for fine grained stuff)
            void create(Device& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
            //image and its memory
            void copy_buffer_to_image(CommandBuffer& command_buffer, void* data, uint32_t width, uint32_t height);
            const VkImage& get_image() const {return _image;}
            const VkDeviceMemory& get_image_memory() const {return _image_memory;}
            const VkImageView& get_image_view() const {return _image_view;}
            private:
            VkImage _image;
            VkDeviceMemory _image_memory;
            VkImageView _image_view;
            //int32_t _image_width;
            //int32_t _image_height;
            //int32_t _image_channels;

            Device& _device;
        };
        Image load_image_from_file(std::string& path);
        void transition_image_layout(Device& device, Image& image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
        void copy_buffer_to_image(Buffer& buffer, Image& image, uint32_t width, uint32_t height);


}
