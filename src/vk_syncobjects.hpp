#pragma once
#include "vk_device.hpp"
#include "common.hpp"


namespace HelloTriangle {
    class Semaphore {
    public:
        Semaphore();
        ~Semaphore();
        void create(Device& device);
        void destroy();
        VkSemaphore& get_semaphore() {return _semaphore;}
    private:
        //One-time creation so we dont need a device reference.
        VkSemaphore _semaphore;
    };

    class Fence {
    public:
        Fence();
        ~Fence();
        void create(Device& device); //lets just use a constructor instead?
        VkFence& get_fence() {return _fence;}
    private:
        VkFence _fence;
    };

    class SyncObjectBuilder {
        public:
        SyncObjectBuilder(VkDevice& device_arg);
        ~SyncObjects();

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
