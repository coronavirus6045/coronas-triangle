#include "common.hpp"

#ifdef NDEBUG
    const bool enableValidationLayers = false; 
#else
    const bool enableValidationLayers = true;
#endif

namespace HelloTriangle {


const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pcreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    class debug_setup{
        public:
        debug_setup();
        private:
        bool checkValidationLayerSupport();
        
    };
}