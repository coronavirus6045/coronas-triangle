#include "vk_syncobjects.hpp"

using HelloTriangle::SyncObjectMaker;
using HelloTriangle::Semaphore;
using HelloTriangle::Fence;

Semaphore SyncObjectMaker::create_semaphore() {
    VkSemaphore semaphore;

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    CHECK_FOR_VK_RESULT(vkCreateSemaphore(_device.get_device(), &semaphore_info, nullptr, &semaphore), "SEMAPHORE CREATE ERROR");

    return (Semaphore) semaphore;
}


Fence SyncObjectMaker::create_fence() {
    VkFence fence;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    CHECK_FOR_VK_RESULT(vkCreateFence(_device.get_device(), &fence_info, nullptr, &fence), "FENCE CREATE ERROR");

    return (Fence) fence;
}

void SyncObjectMaker::delete_sync_object(Fence fence, Semaphore semaphore) {
    if (fence != nullptr) {
        vkDestroyFence(_device.get_device(), (VkFence) fence, nullptr);
    } else if (semaphore != nullptr) {
        vkDestroySemaphore(_device.get_device(), (VkSemaphore) semaphore, nullptr);
    }
}

