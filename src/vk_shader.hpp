#pragma once

#include "common.hpp"
#include "vk_device.hpp"
#define IMGUI_IMPL_VULKAN_USE_VOLK



namespace HelloTriangle {
class Shader {
    public:
        Shader();
        Shader(Device& device, std::string file_path, std::string name);
        ~Shader();
        VkShaderModule& get_shader_module() { return _shader_module; }
        std::string& get_name() {return _name;}
    private:
        VkShaderModule _shader_module;
        std::string _name;

        Device& _device;
};

} // namespace HelloTriangle
