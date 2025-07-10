#include "vk_image.hpp"
#include "common.hpp"
#include "vk_buffers.hpp"
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

using namespace HelloTriangle;
using HelloTriangle::Image;
using HelloTriangle::Buffer;
using HelloTriangle::find_memory_type;

//should i do it
//This is a wrapper for crating native API image
//This may be placed on renderer backend
Image HelloTriangle::load_image_from_file(Device& device, std::string& path) {
    int32_t image_width = 0;
    int32_t image_height = 0;
    int32_t image_channels = 0;

    stbi_uc* pixels = stbi_load(path.c_str(), &image_width, &image_height, &image_channels, STBI_rgb_alpha);
    
    VkDeviceSize image_size = image_width * image_height * 4;

    Image image;

    if (!pixels) {
        throw std::runtime_error("Image texture error.");
    }

    Buffer staging_buffer(device, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    staging_buffer.map_memory((void*) pixels, image_size, 0);
    staging_buffer.unmap_memory();

    stbi_image_free(pixels);

    image.create(device, image_width, image_height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    copy_buffer_to_image(device, staging_buffer, image, image_width, image_height);
    transition_image_layout(device, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    // thanks to destructors, staging buffer is bye byee
}
void Image::create(Device& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.height = static_cast<uint32_t>(height);
    image_info.extent.width = static_cast<uint32_t>(width);
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

    CHECK_FOR_VK_RESULT(vkCreateImage(device.get_device(), &image_info, nullptr, &_image), "")

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(_device->get_device(), _image, &memory_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = memory_requirements.size;
    alloc_info.memoryTypeIndex = HelloTriangle::find_memory_type(device, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    CHECK_FOR_VK_RESULT(vkAllocateMemory(device.get_device(), &alloc_info, nullptr, &_image_memory), "")

    vkBindImageMemory(device.get_device(), _image, _image_memory, 0);

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

    CHECK_FOR_VK_RESULT(vkCreateImageView(device.get_device(), &view_info, nullptr, &_image_view), "")
    //sampler
    _image_width = width;
    _image_height = height;
    _device = &device;
}
//pisney dixar
void transition_image_layout(Device& device, CommandBuffer& command_buffer, Image image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout) {
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

    VkPipelineStageFlags src_stage;
    VkPipelineStageFlags dst_stage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        THROW_RUNTIME_ERROR("");
    }

    //long ass parameters
    vkCmdPipelineBarrier(command_buffer.get(),
                         src_stage, dst_stage, 0, 0, nullptr, 0,
                         nullptr, 1, &barrier
                         );
}

void Image::copy_buffer_to_image(CommandPool& command_pool, Buffer& buffer, uint32_t width, uint32_t height) {
    VkDeviceSize size = width * height * 4;
    //Buffer buffer(_device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    CommandBuffer copy_buffer(*_device, command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    copy_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
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

    vkCmdCopyBufferToImage(copy_buffer.get(), buffer.buffer(), _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    copy_buffer.end();
    // lets overload submit()
    copy_buffer.submit(nullptr, nullptr, nullptr, nullptr);
}
