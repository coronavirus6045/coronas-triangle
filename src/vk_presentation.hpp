#pragma once

#include "common.hpp"
//#include "vk_image.hpp"
#include "vk_device.hpp"
#include "vk_syncobjects.hpp"

namespace HelloTriangle {
    class Swapchain {
    public:
    Swapchain();
    Swapchain(Device& device, VkSurfaceKHR& surface, VkFormat format, VkPresentModeKHR mode, VkExtent2D window_extent);
    // needs constructor that calls create()
    ~Swapchain();
    void create(Device& device, VkSurfaceKHR& surface, VkFormat format, VkPresentModeKHR mode, VkExtent2D window_extent);
    VkSwapchainKHR& get() {return _swapchain;}
    VkResult acquire_next_image(Semaphore signal_semaphore, Fence signal_fence);
    VkResult present(std::vector<Semaphore> signal_semaphores);
    std::vector<VkImage>& get_images() { return _swapchain_images; }
    std::vector<VkImageView>& get_image_views() {return _swapchain_views;}
    VkFormat get_format() {return _format;}
    uint32_t get_image_width() { return _extent.width; }
    uint32_t get_image_height() { return _extent.height; }
    uint32_t get_image_index() { return _image_index; }

private:
    VkSwapchainKHR _swapchain;
    std::vector<VkImage> _swapchain_images;
    std::vector<VkImageView> _swapchain_views;
    uint32_t _image_index;

    VkExtent2D _extent;
    VkPresentModeKHR _mode;
    VkFormat _format;

    Device* _device;
    };

}
