#include "vk_instance.hpp"
#include <cstdint>
#include <vector>


using HelloTriangle::create_instance;


create_instance::create_instance(const std::string& app_name, const std::vector<const char*>& enabled_extensions) {
    VkApplicationInfo appInfo{}; //Fill in this struct for information about app (in case of driver optimizations)
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; //Structure type
        appInfo.pApplicationName = "Hey"; //App name
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0); //App version
        appInfo.pEngineName = "coronas-triangle"; //Engine name
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0); //Engine version
        appInfo.apiVersion = VK_API_VERSION_1_1; //Vk API version

    VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; //Structure type
        createInfo.pApplicationInfo = &appInfo; //pointer to app info

        createInfo.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size()); //Enabled extension count
        createInfo.ppEnabledExtensionNames = enabled_extensions.data(); //Extension count
    
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE INSTANCE!!!!");
    }

    volkLoadInstance(instance);
}

VkInstance& create_instance::get_instance() {
    return instance;
}
