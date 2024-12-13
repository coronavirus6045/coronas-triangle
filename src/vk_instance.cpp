

#include "vk_instance.hpp"
#include "vk_debugmsg.hpp"



using HelloTriangle::create_instance;



create_instance::create_instance(const std::string& app_name, const std::vector<const char*> enabled_extensions, PFN_vkDebugUtilsMessengerCallbackEXT debugCallback) {
    volkInitialize();
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
    
    if (enableValidationLayers && debugCallback != nullptr) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        debug_setup::fill_debug_msg_info(debugCreateInfo, debugCallback);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }
    auto extensions = debug_setup::get_extensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();


    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE INSTANCE!!!!");
    }

//    if (enableValidationLayers && debug_setup::check_validation_support()) {
//        throw std::runtime_error("Well the validation layer you gave is NOT there!!!!!");
//    }

    volkLoadInstance(instance);
}

create_instance::~create_instance() {
    vkDestroyInstance(instance, nullptr);
}




