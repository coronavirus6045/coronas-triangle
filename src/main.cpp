

#include "common.hpp"
#include "init_window.hpp"
#include "vk_device.hpp"
#include "vk_instance.hpp"
#include "vk_debugmsg.hpp"
#include "vk_presentation.hpp"
#include "vk_pipeline.hpp"
#include "vk_command.hpp"
#include "vk_syncobjects.hpp"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <exception>
#include <vulkan/vulkan_core.h>


HelloTriangle::init_window window_object(800, 600, "Vulkan");
HelloTriangle::create_instance instance_object("triangle", HelloTriangle::deviceExtensions, HelloTriangle::debug_setup::debugCallback);
HelloTriangle::debug_setup debug_msg_object(instance_object.get_instance(), HelloTriangle::validationLayers);
HelloTriangle::create_device device_object;
HelloTriangle::presentation_setup presentation_object(device_object.get_device(), instance_object.get_instance(), window_object.get_window());
HelloTriangle::pipeline_state pipeline_object(device_object.get_device());
HelloTriangle::command_objects cmd_object(device_object.get_physical_device(), device_object.get_device(), presentation_object.get_surface());
HelloTriangle::sync_objects sync_obj(device_object.get_device());


struct context {
    std::vector<VkFence> inFlightFences;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    uint32_t currentFrame = 0;

    GLFWwindow* window;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkRenderPass renderPass;
    VkSwapchainKHR swapchain;
    std::vector<VkCommandBuffer> commandBuffers;
    VkExtent2D swapchainExtent;
    std::vector<VkFramebuffer> swapchainFramebuffers;
    VkPipeline graphicsPipeline;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

};

context context_info;

void init() {
    //https://i.kym-cdn.com/photos/images/original/002/914/678/a6a.png
    //imma do a struct for objects so oject creation funcs can read and use it as params;
    //window, instance, and debug stuff is already created after object init
    //surface first then device since screwups may happen
    presentation_object.create_surface(instance_object.get_instance(), window_object.get_window());
    device_object.find_physical_device(instance_object.get_instance(), presentation_object.get_surface());
    device_object.create_logical_device(presentation_object.get_surface());
    presentation_object.create_swapchain(device_object.get_physical_device(), device_object.get_device());
    presentation_object.create_image_views(device_object.get_device());
    pipeline_object.create_render_pass(presentation_object.get_format());
    pipeline_object.create_graphics_pipeline(presentation_object.get_extent());
    presentation_object.create_framebuffers(pipeline_object.get_render_pass());
    cmd_object.create_command_pool();
    cmd_object.create_command_buffer();
    sync_obj.create_sync_objects();
    //sync objects already created
    context_info = {
        .inFlightFences = sync_obj.get_in_flight_fences(),
        .renderFinishedSemaphores = sync_obj.get_render_finished_semaphores(),
        .imageAvailableSemaphores = sync_obj.get_image_available_semaphores(),
        .window = window_object.get_window(),
        .physicalDevice = device_object.get_physical_device(),
        .device = device_object.get_device(),
        .renderPass = pipeline_object.get_render_pass(),
        .swapchain = presentation_object.get_swap_chain(),
        .commandBuffers = cmd_object.get_command_buffers(),
        .swapchainExtent = presentation_object.get_extent(),
        .swapchainFramebuffers = presentation_object.get_framebuffers(),
        .graphicsPipeline = pipeline_object.get_graphics_pipeline(),
        .graphicsQueue = device_object.get_graphics_queue(),
        .presentQueue = device_object.get_present_queue()
    };
    //next incarnation will be a proper design
    //keep on calling getter funcs and i will fight you - deity of coding
    //Crazy ass
    //worst C++ code
    //pray to God if this is gonna work

}




void draw(context* c) {
    
    vkWaitForFences(c->device, 1, &c->inFlightFences[c->currentFrame], VK_TRUE, UINT64_MAX);
        uint32_t imageIndex;

        VkResult result = vkAcquireNextImageKHR(c->device, c->swapchain, UINT32_MAX, c->imageAvailableSemaphores[c->currentFrame], nullptr, &imageIndex);
        
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            presentation_object.recreate_swap_chain(c->physicalDevice, c->device, c->window, c->renderPass);
            window_object.framebufferResized = false;
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("recreate error");
        }
        vkResetFences(c->device, 1, &c->inFlightFences[c->currentFrame]);
        
        vkResetCommandBuffer(c->commandBuffers[c->currentFrame], 0);
        cmd_object.record_command_buffer(c->commandBuffers[c->currentFrame], c->renderPass, c->swapchainExtent, c->swapchainFramebuffers, imageIndex, c->graphicsPipeline);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {c->imageAvailableSemaphores[c->currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};  
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &c->commandBuffers[c->currentFrame];

        VkSemaphore signalSemaphores[] = {c->renderFinishedSemaphores[c->currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(c->graphicsQueue, 1, &submitInfo, c->inFlightFences[c->currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("WE CANT SUBMIT CMDBUFFER!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapChains[] = {c->swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

       result = vkQueuePresentKHR(c->presentQueue, &presentInfo); //FINALLY

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            window_object.framebufferResized = false;
            presentation_object.recreate_swap_chain(c->physicalDevice,c->device, c->window, c->renderPass);
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("");
        }

        c->currentFrame = (c->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void main_loop() {
    while (!glfwWindowShouldClose(window_object.get_window())) {
        glfwPollEvents();
        draw(&context_info);
    }
}


int main(int argc, char** argv){
    try {
        std::cout << "[INIT]: Starting the triangle.....\n";
        
        init();
        main_loop();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//DOES IT WORK WELL?