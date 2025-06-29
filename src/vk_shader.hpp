#pragma once

#include "common.hpp"
#include "vk_device.hpp"

namespace HelloTriangle {
class Shader {
    public:
    Shader();
    Shader(Device& device, std::string file_path);
    ~Shader();
    VkShaderModule& get() {return _shader_module;}
    private:
    VkShaderModule _shader_module;

    Device& _device;
};

}
