#include "common.hpp"
#include <GLFW/glfw3.h>

namespace HelloTriangle {
    const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class presentation_setup {
        public:
        void create_surface(VkInstance instance, GLFWwindow* window);
        SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice device);
        void create_swapchain();

        private:
        VkSurfaceKHR surface;     
        VkSwapchainKHR swapchain;

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    };
}