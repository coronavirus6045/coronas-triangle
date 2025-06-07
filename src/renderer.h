#pragma once
#include "common.hpp"
#include "vk_device.hpp"
#include "vk_pipeline.hpp"
#include "vk_presentation.hpp"

//this is the master renderer class that will do all the work
class Renderer {
    public:
        Renderer();
        ~Renderer();

        void initialize();
        void draw();
    private:
        //screw it, allocate on the heap
        std::unique_ptr<HelloTriangle::Device> _device;
        std::unique_ptr<HelloTriangle::Swapchain> _swapchain;
        std::unique_ptr<HelloTriangle::PipelineAllocator> _pipeline;

        std::unique_ptr<HelloTriangle::command_objects::CommandPool> _command_pool;
        std::unique_ptr<HelloTriangle::command_objects::CommandBuffer> _command_buffer;;//long ass name

        std::unique_ptr<>
};
