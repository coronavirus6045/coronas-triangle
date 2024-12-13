#pragma once

#include "common.hpp"


namespace HelloTriangle {
    class sync_objects {
        public:
        sync_objects(VkDevice& device_arg);
        ~sync_objects();

        void create_sync_objects();
        std::vector<VkSemaphore>& get_image_available_semaphores() {return imageAvailableSemaphores;}
        std::vector<VkSemaphore>& get_render_finished_semaphores() {return renderFinishedSemaphores;}
        std::vector<VkFence>& get_in_flight_fences() {return inFlightFences;}
        private:
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        VkDevice& device;
    };
}