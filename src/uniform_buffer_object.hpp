#include "common.hpp"

namespace HelloTriangle {
struct uniform_buffer_object {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};
}