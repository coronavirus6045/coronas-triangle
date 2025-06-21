#pragma once
#include "vk_device.hpp"
#include "common.hpp"


namespace HelloTriangle {
//
    typedef VkFence Fence;
    typedef VkSemaphore Semaphore;

    class SyncObjectMaker {
        public:
            SyncObjectMaker(Device& device);
            ~SyncObjectMaker();

            Fence create_fence();
            Semaphore create_semaphore();

            void delete_sync_object(Fence fence, Semaphore semaphore);

        private:
            Device& _device;
    };
}
