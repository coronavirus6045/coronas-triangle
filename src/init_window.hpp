#pragma once

#include "common.hpp"


const uint32_t WIDTH = 800; //Window size
const uint32_t HEIGHT = 600; //Window size


namespace HelloTriangle {
    class init_window {
        public:
        init_window(int init_width, int init_height, const char* window_title);
        GLFWwindow* get_window() {return window;}
        void destroy_window();
        bool framebufferResized = false;
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
        private:
        int width;
        int height;
        const char* title;
        GLFWwindow* window;
    };
}