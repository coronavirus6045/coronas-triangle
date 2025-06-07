#pragma once

#include "common.hpp"
#include "vk_device.hpp"

namespace HelloTriangle {
class Shader {
    public:
    Shader(Device& device, std::string file_path);
    ~Shader();
    VkShaderModule& get() {return p_shader_module;}
    private:
    VkShaderModule p_shader_module;

    Device& p_device;
};

}
