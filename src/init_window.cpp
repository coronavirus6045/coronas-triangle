#include "init_window.hpp"
#include <GLFW/glfw3.h>

using HelloTriangle::Window;
//ill put at main()
Window::Window(int init_width, int init_height, const char* window_title) : width(init_width), height(init_height), title(window_title) {
    glfwInit();
    SDL_Init(SDL_INIT_EVERYTHING); //idc

    SDL_Window* window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, (SDL_WindowFlags) SDL_WINDOW_VULKAN);
    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface(window, instance, )

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

void init_window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        app ->framebufferResized = true;
}
