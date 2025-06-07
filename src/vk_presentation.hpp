#pragma once

#include "common.hpp"
#include "vk_image.hpp"


namespace HelloTriangle {
    const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class Swapchain {
    public:
    Swapchain();
    ~Swapchain();
    void create(VkSurfaceKHR& surface, VkFormat format, VkPresentModeKHR mode);
    void recreate(SDL_Window* window, ); //only native handles i could use are window and surface
    void destroy();
    private:

    VkSwapchainKHR _swapchain;
    std::vector<VkImage> _swapchain_images;
    std::vector<VkImageView> _swapchain_views;
    VkExtent2D _extent;
    VkPresentModeKHR _mode;
    VkFormat _format;
    Device& _device;
    VkSurfaceKHR& _surface;
    };

    class presentation_setup {
        public:
        presentation_setup(VkDevice& device_arg, VkInstance& instance, GLFWwindow* window_arg);
        ~presentation_setup();
        void create_surface(VkInstance instance, GLFWwindow* window);
        static SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface);
        void create_swapchain(VkPhysicalDevice physical_device, VkDevice device);
        void create_image_views(VkDevice device);
        void recreate_swap_chain(VkPhysicalDevice physical_device, VkDevice device, GLFWwindow* window, VkRenderPass pass);
        void create_framebuffers(VkRenderPass pass);
        VkSurfaceKHR& get_surface() {return surface;}
        VkExtent2D& get_extent() {return swapChainExtent;}
        VkFormat& get_format() {return swapChainImageFormat;}
        VkSwapchainKHR& get_swap_chain() {return swapchain;}
        std::vector<VkFramebuffer>& get_framebuffers() {return swapChainFramebuffers;}
        //stroke
        private:
        VkDevice& device;
        VkInstance& instance;
        GLFWwindow* window;
        VkSurfaceKHR surface;     
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
        void cleanupSwapChain();
    };
}
