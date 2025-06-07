#include "vk_shader.hpp"

using HelloTriangle::Shader;

Shader::Shader(Device& device, std::string file_path) : p_device(device) {
    std::ifstream shader_file(file_path, std::ios::ate | std::ios::binary);

    if (!shader_file.is_open()) {
        throw std::runtime_error("Joever error: can't read shader file!");
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

    if (vkCreateShaderModule(p_device.device, &createInfo, nullptr, &p_shader_module) != VK_SUCCESS) {
        throw std::runtime_error("Joever error: Can't make shader module, womp womp.");
    }
}
