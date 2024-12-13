#include "init_window.hpp"
#include <GLFW/glfw3.h>

using HelloTriangle::init_window;

init_window::init_window(int init_width, int init_height, const char* window_title) : width(init_width), height(init_height), title(window_title) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}
