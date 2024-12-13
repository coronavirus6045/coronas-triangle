#include "vk_debugmsg.hpp"

using HelloTriangle::debug_setup;

debug_setup::debug_setup(VkInstance& instance_arg, const std::vector<const char*>& validation_layers) : 
    instance(instance_arg),
    validationLayers(validation_layers) {
    if (!enableValidationLayers) return;
        
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        fill_debug_msg_info(createInfo, debugCallback);
        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)){
            throw std::runtime_error("Sorry bud, its joever, debug messenger can't rise up.");
        }
    
}

std::vector<const char*> debug_setup::get_extensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
}

VkResult debug_setup::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT*
    pcreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pcreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

void debug_setup::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const 
    VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func !=nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

bool debug_setup::check_validation_support() {
    uint32_t layerCount; //Validation layer count
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        //Above calls this function to get how many validation layers are available

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        //same but with property names

        //compare requested validation layers with available ones.
        for (const char* layerName : HelloTriangle::validationLayers) {
            bool layerFound = false;
            //below is a for statement meant for arrays         
            for (const auto& layerProperties : availableLayers) {
                
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            if (!layerFound) {
                return false;
                }
            }
        }
        return true;
}

void debug_setup::fill_debug_msg_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo, PFN_vkDebugUtilsMessengerCallbackEXT callback) {
    createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = callback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debug_setup::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
        std::cerr << "[VALIDATION]:" << pCallbackData->pMessage << std::endl;
        
        return VK_FALSE;
}



debug_setup::~debug_setup() {
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
}