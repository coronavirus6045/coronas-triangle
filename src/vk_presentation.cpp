#include "vk_presentation.hpp"

using HelloTriangle::presentation_setup;
using HelloTriangle::deviceExtensions;
using HelloTriangle::SwapChainSupportDetails;

void presentation_setup::create_surface(VkInstance instance, GLFWwindow* window) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Window surface error: \"I failed, son.\"");
    }
}

