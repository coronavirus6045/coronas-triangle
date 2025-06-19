#include "vk_command.hpp"
#include "common.hpp"
#include "scene/scene_obj.hpp"
#include "vk_device.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>
using HelloTriangle::command_objects::CommandBuffer;
using HelloTriangle::command_objects::CommandPool;
using HelloTriangle::device_object;

//command_objects::command_objects(VkPhysicalDevice& physical_device, VkDevice& device_arg, VkSurfaceKHR& surface_arg) : physicalDevice(physical_device), device(device_arg), surface(surface_arg) {}

CommandPool::CommandPool(device_object& device) : _device(device) {
    QueueFamilyIndices queueFamilyIndices = create_device::findQueueFamilies(_device, surface);

        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(_device.device, &pool_info, nullptr, &_command_pool) != VK_SUCCESS) {
            throw std::runtime_error("JOEVER ERROR SUPER LEVEL: COMMAND POOL FAILED!");
        }
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(_device.device, _command_pool, nullptr);
}

CommandBuffer::CommandBuffer(Device& device, CommandPool& CommandPool, VkCommandBufferLevel level) : _device(device) {

}

void CommandBuffer::create(Device& device, CommandPool& command_pool, VkCommandBufferLevel level) : _device(device), _command_pool(command_pool) {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool.get();
    alloc_info.level = level;
    alloc_info.commandBufferCount = 1;
    
    CHECK_FOR_VK_RESULT(vkAllocateCommandBuffers(_device.get_device(), &alloc_info, &_command_buffer), "")
    _device = device;
}

void CommandBuffer::begin(VkCommandBufferUsageFlags usage) {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = usage;
    begin_info.pInheritanceInfo = nullptr;

    CHECK_FOR_VK_RESULT(vkBeginCommandBuffer(_command_buffer, &begin_info), "")
}

void CommandBuffer::end() {
    CHECK_FOR_VK_RESULT(vkEndCommandBuffer(_command_buffer), "")
}

void CommandBuffer::submit() {
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &_command_buffer;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = ;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = ;

    CHECK_FOR_VK_RESULT(vkQueueSubmit(_device.get_graphics_queue(), 1, &submit_info, ), "")
    vkQueueWaitIdle(_device.get_graphics_queue());
}


//Will now be separate, todo: abstract all VkCmds. but for now use VkCmd*s instead in main()
void command_objects::record_CommandBuffer(uint32_t currentFrame,VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkExtent2D swapChainExtent, std::vector<VkFramebuffer> swapChainFramebuffers, uint32_t imageIndex, VkPipelineLayout pipelineLayout, VkPipeline graphicsPipeline, VkBuffer vertexBuffer, const std::vector<vertex> vertices, VkBuffer indexBuffer, const std::vector<uint32_t> indices, std::vector<VkDescriptorSet> descriptorSets) {
    VkCommandBufferBeginInfo begin_info{};
         begin_info.sType = VK_STRUCTURE_TYPE_CommandBuffer_BEGIN_INFO;
         begin_info.flags = 0;
         begin_info.pInheritanceInfo = nullptr;

         if (vkBeginCommandBuffer(commandBuffer, &begin_info) != VK_SUCCESS) {
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
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
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

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
         //vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
         vkCmdEndRenderPass(commandBuffer);
         
         if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Minor inconvenience: BUFFER RECORD FAIL!");
         }
}



//i lack design stuff 
