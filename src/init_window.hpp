#include "common.hpp"
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800; //Window size
const uint32_t HEIGHT = 600; //Window size
const int MAX_FRAMES_IN_FLIGHT = 2;

namespace HelloTriangle {
    class init_window {
        public:
        init_window(int init_width, int init_height, const char* window_title);
        void destroy_window();
        private:
        int width;
        int height;
        const char* title;
        GLFWwindow* window;
        void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    };
}