#pragma once

#include "common.hpp"



namespace HelloTriangle {
    class create_instance {
        public:
        create_instance(const std::string& app_name, const std::vector<const char*> enabled_extensions, PFN_vkDebugUtilsMessengerCallbackEXT debugCallback);
        ~create_instance();
        VkInstance& get_instance() {return instance;}
        private:
        VkInstance instance;
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    };
}


