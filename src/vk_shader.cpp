#include "vk_shader.hpp"

using HelloTriangle::Shader;

Shader::Shader(Device& device, std::string file_path, std::string name) : _device(device) {
    std::ifstream shader_file(file_path, std::ios::ate | std::ios::binary);

    if (!shader_file.is_open()) {
        THROW_RUNTIME_ERROR("Joever error: can't read shader file!");
    }

    std::size_t file_size = (std::size_t) shader_file.tellg();
    std::vector<char> buffer(file_size);
    shader_file.seekg(0);
    shader_file.read(buffer.data(), file_size);
    shader_file.close();

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buffer.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

    CHECK_FOR_VK_RESULT(vkCreateShaderModule(_device.get_device(), &createInfo, nullptr, &_shader_module), "")
    _name = name;
}

Shader::~Shader() {
    vkDestroyShaderModule(_device.get_device(), _shader_module, nullptr);
}
