#include "common.hpp"
#include <cstdint>
#include <exception>


namespace HelloTriangle {
    class create_instance {
        public:
        create_instance(const std::string& app_name, const std::vector<const char*>& enabled_extensions);
        VkInstance& get_instance();
        private:
        VkInstance instance;
    };
}


