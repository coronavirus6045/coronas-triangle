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
            VkImage& get_image() {return _image;}
            VkDeviceMemory& get_image_memory() {return _image_memory;}
            VkImageView& get_image_view() {return _image_view;}
            uint32_t get_image_width() {return _image_width;}
            uint32_t get_image_height() {return _image_height;}
            private:
            VkImage _image;
            VkDeviceMemory _image_memory;
            VkImageView _image_view;
            uint32_t _image_width;
            uint32_t _image_height;
            //int32_t _image_channels;

            Device& _device;
        };
        Image load_image_from_file(std::string& path);
        void transition_image_layout(Device& device, Image& image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
        void copy_buffer_to_image(Buffer& buffer, Image& image, uint32_t width, uint32_t height);


}
