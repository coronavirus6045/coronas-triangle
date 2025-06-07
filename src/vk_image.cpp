#include "vk_image.hpp"
#include "src/common.hpp"
#include "src/vk_buffers.hpp"
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

using namespace coronas_triangle;
using namespace HelloTriangle;
using image_creation::ImageTexture;
using buffer_creation::Buffer;
using buffer_creation::find_memory_type;


void ImageTexture::load_image_file(std::string& path) {
    stbi_uc* pixels = stbi_load(path.c_str(), &_image_width, &_image_height, &_image_channels, STBI_rgb_alpha);
    
    VkDeviceSize image_size = _image_width * _image_height * 4;

    if (!pixels) {
        throw std::runtime_error("Image texture error.");
    }

    Buffer staging_buffer(_device, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    staging_buffer.map_memory((void*) pixels, image_size, 0);
    staging_buffer.unmap_memory();

    stbi_image_free(pixels);

    create_image(_image_width, _image_height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}
void ImageTexture::create_image(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.height = static_cast<uint32_t>(_image_height);
    image_info.extent.width = static_cast<uint32_t>(_image_width);
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1; 
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.flags = 0;

    if (vkCreateImage(_device.device, &image_info, nullptr, &_image) != VK_SUCCESS) {
        throw std::runtime_error("Image error");
    }

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(_device.device, _image, &memory_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = memory_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(_device, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(p_device.device, &alloc_info, nullptr, &_image_memory) != VK_SUCCESS) {
        throw std::runtime_error("");
    }

    vkBindImageMemory(p_device.device, _image, _image_memory, 0);

    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = _image;
    view_info.format = format;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 0;


    if (format != VK_FORMAT_D32_SFLOAT) {
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    } else {
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    if (vkCreateImageView(p_device.device, &view_info, nullptr, &_image_view) != VK_SUCCESS) {

    }
}
//pisney dixar
void image_creation::transition_image_layout(Device& device, ImageTexture image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout) {
    command_objects::CommandBuffer transition_command_buffer(device, command_pool, );

    transition_command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image.get_image();
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    vkCmdPipelineBarrier(transition_command_buffer, );
    transition_command_buffer.end();
    transition_command_buffer.submit();
}

void copy_buffer_to_image(Device& device, Buffer& buffer, ImageTexture& image, uint32_t width, uint32_t height) {
    command_objects::CommandBuffer copy_command_buffer(device, command_pool, );

    copy_command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(copy_command_buffer.get(), buffer.buffer(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);


    copy_command_buffer.end();
    copy_command_buffer.submit();
}
