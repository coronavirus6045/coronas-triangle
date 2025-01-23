#include "vk_command.hpp"
#include "common.hpp"
#include "scene/scene_obj.hpp"
#include "vk_device.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>
using HelloTriangle::command_objects;

command_objects::command_objects(VkPhysicalDevice& physical_device, VkDevice& device_arg, VkSurfaceKHR& surface_arg) : physicalDevice(physical_device), device(device_arg), surface(surface_arg) {}

void command_objects::create_command_pool() {
    QueueFamilyIndices queueFamilyIndices = create_device::findQueueFamilies(physicalDevice, surface);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("JOEVER ERROR SUPER LEVEL: COMMAND POOL FAILED!");
        }
}

void command_objects::create_command_buffer() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data())) {
    throw std::runtime_error("JOEVER ERROR FATAL: COMMAND BUFFER FAILED!");
    }
}

void command_objects::record_command_buffer(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkExtent2D swapChainExtent, std::vector<VkFramebuffer> swapChainFramebuffers, uint32_t imageIndex, VkPipeline graphicsPipeline, VkBuffer vertexBuffer, const std::vector<vertex> vertices) {
    VkCommandBufferBeginInfo beginInfo{};
         beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
         beginInfo.flags = 0;
         beginInfo.pInheritanceInfo = nullptr;

         if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Due to copyright, we can't record buffer.");
         }

         //START
         VkRenderPassBeginInfo renderPassInfo{};
         renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
         renderPassInfo.renderPass = renderPass;
         renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
         renderPassInfo.renderArea.offset = {0, 0};
         renderPassInfo.renderArea.extent = swapChainExtent;

         VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
         renderPassInfo.clearValueCount = 1;
         renderPassInfo.pClearValues = &clearColor;
         vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
         
         vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
         
         VkBuffer vertexBuffers[] = {vertexBuffer};
         VkDeviceSize offsets[] = {0};
         vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

         VkViewport viewport {};
         viewport.x = 0.0f;
         viewport.y = 0.0f;
         viewport.width = static_cast<float>(swapChainExtent.width);
         viewport.height = static_cast<float>(swapChainExtent.height);
         viewport.minDepth = 0.0f;
         viewport.maxDepth = 1.0f;
         vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

         VkRect2D scissor{};
         scissor.offset = {0, 0};
         scissor.extent = swapChainExtent;
         vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

         vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

         vkCmdEndRenderPass(commandBuffer);
         
         if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Minor inconvenience: BUFFER RECORD FAIL!");
         }
}

command_objects::~command_objects() {
    vkDestroyCommandPool(device, commandPool, nullptr);
}

//i lack design stuff 