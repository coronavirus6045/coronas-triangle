#pragma once

#include "common.hpp"

#define IMGUI_IMPL_VULKAN_USE_VOLK


namespace HelloTriangle {
    class Instance {
    public:
    Instance(const std::string& app_name, const std::vector<const char*> enabled_extensions);
    ~Instance();
    void create(const std::string& app_name, const std::vector<const char*> enabled_extensions);
    VkInstance& get() {return _instance;}
    private:
    VkInstance _instance;
    };
}


