#pragma once

#include "common.hpp"
#include "vk_instance.hpp"




namespace HelloTriangle {


const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};


    class debug_setup{
        public:
        debug_setup(VkInstance& instance_arg, const std::vector<const char*>& validationLayers);
        ~debug_setup();
        //sorry for inconsistency
        static std::vector<const char*> get_extensions();
        static bool check_validation_support();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
        static void fill_debug_msg_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo, PFN_vkDebugUtilsMessengerCallbackEXT callback);

//        friend class HelloTriangle::create_instance;
        private:
        VkDebugUtilsMessengerEXT debugMessenger;
        PFN_vkDebugUtilsMessengerCallbackEXT callback = debugCallback;
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pcreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

        VkInstance& instance;
        const std::vector<const char*>& validationLayers;
    };
}