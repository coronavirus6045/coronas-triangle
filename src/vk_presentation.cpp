5#include "vk_presentation.hpp"
#include "vk_device.hpp"


using HelloTriangle::presentation_setup;
using HelloTriangle::Swapchain;
//#if 0 //TODO: what am i gonna do with this? Do i initialize all variables needed for the member functions?
//How do i deal with stuff that needs access to other parts(classes)
presentation_setup::presentation_setup(VkDevice& device_arg, VkInstance& instance_arg, GLFWwindow* window_arg) : device{device_arg}, instance(instance_arg), window(window_arg) {

}
//#endif

void presentation_setup::create_surface(VkInstance instance, GLFWwindow* window) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Window surface error: \"I failed, son.\"");
    }
}

//VK_FORMAT_B8G8R8_SRGB
void Swapchain::create(VkSurfaceKHR& surface, VkFormat format, VkPresentModeKHR mode) {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> modes;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device.get_physical_device(), surface, &capabilities);

    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(_device.get_physical_device(), surface, &format_count, nullptr);

    if (format_count != 0) {
        formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(_device.get_physical_device(), surface, &format_count, formats.data());
    }

    uint32_t mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(_device.get_physical_device(), surface, &mode_count, nullptr);

    if (mode_count != 0) {
        modes.resize(mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(_device.get_physical_device(), surface, &mode_count, modes.data());
    }

    uint32_t image_count = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
       image_count = capabilities.maxImageCount;
    }


        VkSwapchainCreateInfoKHR swapchain_info{};
        swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_info.surface = surface;
        swapchain_info.minImageCount = image_count;


        for (const auto& i : formats) {
            if (i.format == format) {
                swapchain_info.imageFormat = i.format;
                swapchain_info.imageColorSpace = i.colorSpace;
                break;
            }
            swapchain_info.imageFormat = formats[0].format;
            swapchain_info.imageColorSpace = formats[0].colorSpace;
        }

        for (const auto& i : modes) {
            if (i == mode) {
                swapchain_info.presentMode = mode;
                break;
            }
            swapchain_info.presentMode = modes[0];
        }

        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            _extent = capabilities.currentExtent;
        } else {
            int width, height;
            //glfwGetFramebufferSize(window, &width, &height);

            _extent.width = std::clamp((uint32_t) width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            _extent.height = std::clamp((uint32_t) height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        }

        uint32_t indices[] = {_device.get_queue_family()};

        swapchain_info.imageExtent = _extent;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_info.queueFamilyIndexCount = 2;
        swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_info.pQueueFamilyIndices = indices;

        swapchain_info.preTransform = capabilities.currentTransform;

        swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        swapchain_info.clipped = VK_TRUE;

        swapchain_info.oldSwapchain = VK_NULL_HANDLE;
        
        if (vkCreateSwapchainKHR(_device.get_device(), &swapchain_info, nullptr, &_swapchain)) {
            throw std::runtime_error("Joever Error: Swap chain failed to cook");
        }
        vkGetSwapchainImagesKHR(_device.get_device(), _swapchain, &image_count, nullptr);
        _swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(_device.get_device(), _swapchain, &image_count, _swapchain_images.data());
        _swapchain_views.resize(_swapchain_images.size());

        for (size_t i = 0; i < _swapchain_images.size(); i++) {
            VkImageViewCreateInfo view_info{};
            view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            view_info.image = _swapchain_images[i];
            view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            view_info.format = format;
            view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            view_info.subresourceRange.baseMipLevel = 0;
            view_info.subresourceRange.levelCount = 1;
            view_info.subresourceRange.baseArrayLayer = 0;
            view_info.subresourceRange.layerCount = 1;

            if (vkCreateImageView(_device.get_device(), &view_info, nullptr, &_swapchain_views[i]) != VK_SUCCESS) {
                throw std::runtime_error("Joever Error: vkCreateImageView failed.");
            }
        }

        _surface = surface;
        _format = format;
        _mode = mode;
}

void presentation_setup::cleanupSwapChain() {
    for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
            vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
        }

        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            vkDestroyImageView(device, swapChainImageViews[i], nullptr);
        }
    vkDestroySwapchainKHR(device, swapchain, nullptr);
}

void Swapchain::recreate(VkPhysicalDevice physical_device, VkDevice device, GLFWwindow* window, VkRenderPass pass) {
    int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            swapChainExtent.width = width;
            swapChainExtent.height = height; //shenanigans
            glfwWaitEvents();          
        }

        vkDeviceWaitIdle(device);
        
        destroy();
        
        create(_surface, _format, _mode);
        create_framebuffers(pass);
}

void presentation_setup::create_framebuffers(VkRenderPass pass) {
    swapChainFramebuffers.resize(swapChainImageViews.size());
        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            VkImageView attachments[] = {
                swapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = pass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Fatal Joever ERROR:");
            }
        
        }
}

presentation_setup::~presentation_setup() {
    cleanupSwapChain();
}
