#include "vk_instance.hpp"
#include "vk_debugmsg.hpp"

using HelloTriangle::Instance;

void Instance::create(const std::string& app_name, const std::vector<const char*> enabled_extensions, PFN_vkDebugUtilsMessengerCallbackEXT debugCallback) {
    volkInitialize();
    VkApplicationInfo app_info{}; //Fill in this struct for information about app (in case of driver optimizations)
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; //Structure type
        app_info.pApplicationName = app_name.c_str(); //App name
        app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 0); //App version
        app_info.pEngineName = "coronas-triangle"; //Engine name
        app_info.engineVersion = VK_MAKE_VERSION(0, 0, 0); //Engine version
        app_info.apiVersion = VK_API_VERSION_1_2; //Vk API version

    VkInstanceCreateInfo instance_info{};
        instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; //Structure type
        instance_info.pApplicationInfo = &app_info; //pointer to app info

        instance_info.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size()); //Enabled extension count
        instance_info.ppEnabledExtensionNames = enabled_extensions.data(); //Extension count
    
    if (enableValidationLayers && debugCallback != nullptr) {
        instance_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instance_info.ppEnabledLayerNames = validationLayers.data();

        //debug_setup::fill_debug_msg_info(debugCreateInfo, debugCallback);
        //instance_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        instance_info.enabledLayerCount = 0;

        instance_info.pNext = nullptr;
    }
    auto extensions = debug_setup::get_extensions();
    instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instance_info.ppEnabledExtensionNames = extensions.data();


    CHECK_FOR_VK_RESULT(vkCreateInstance(&instance_info, nullptr, &_instance), "Unable to create instance!")

//    if (enableValidationLayers && debug_setup::check_validation_support()) {
//        throw std::runtime_error("Well the validation layer you gave is NOT there!!!!!");


    volkLoadInstance(_instance);
}

Instance::~Instance() {
    vkDestroyInstance(_instance, nullptr);
}




