#pragma once
#include "common.hpp"
#include "vk_device.hpp"

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
        Device* _device;
};
} // namespace HelloTriangle
