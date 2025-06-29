#pragma once
#include "common.hpp"
#include "vk_device.hpp"
#include "vk_pipeline.hpp"
#include "vk_presentation.hpp"
#include "vk_syncobjects.hpp"
#include "vk_command.hpp"
#include "vk_instance.hpp"
#include "vk_buffers.hpp"
#include "vk_shader.hpp"
#include "vk_descriptor.hpp"
#include "scene/scene_obj.hpp"
//We include vk_image later

//this is the master renderer class that will do all the work


class Renderer {
    public:
        //Renderer();
        //~Renderer();
        void boot();
        void initialize();
        void draw();
        void swapchain_resize();
        void uniform_update(VkExtent2D extent);
        VkExtent2D window_size_to_extent();
    private:
        bool feel_like_it_wants_to_exit = false;
        bool feel_like_it_wants_to_stop_rendering = false;
        // Window and surface
        SDL_Window* _window;
        VkSurfaceKHR _surface;

        // Frame index
        uint32_t _frame_index = 0;

        // Instance and device
        HelloTriangle::Instance* _instance;
        HelloTriangle::Device* _device;

        // Swapchain and images
        HelloTriangle::Swapchain* _swapchain;
        std::vector<VkImage> _swapchain_images;

        // Main pipeline and synchronization maker
        HelloTriangle::PipelineMaker* _pipeline_maker;
        HelloTriangle::SyncObjectMaker* _sync_object_maker;

        // Sync objects
        std::array<HelloTriangle::Fence, MAX_FRAMES_IN_FLIGHT> _main_fence;
        std::array<HelloTriangle::Semaphore, MAX_FRAMES_IN_FLIGHT> _image_semaphore;
        std::array<HelloTriangle::Semaphore, MAX_FRAMES_IN_FLIGHT> _render_semaphore;

        // Main command_pool
        HelloTriangle::CommandPool* _command_pool;

        // Descriptor layout, pool, sets
        std::vector<HelloTriangle::DescriptorLayout> _descriptor_layout;
        HelloTriangle::DescriptorPool* _descriptor_pool;
        HelloTriangle::PipelineLayout* _pipeline_layout;
        std::array<HelloTriangle::DescriptorSet, MAX_FRAMES_IN_FLIGHT> _descriptor_set;

        // Main render pass
        HelloTriangle::RenderPass* _main_pass;

        // All shaders
        std::vector<HelloTriangle::Shader> _shaders;

        // Framebuffer
        std::vector<HelloTriangle::Framebuffer> _main_framebuffer;

        // Pipelines
        std::vector<HelloTriangle::PipelineHandle> _pipelines;

        std::vector<HelloTriangle::CommandBuffer> _main_command_buffer;//long ass name

        // Buffers
        HelloTriangle::Buffer* _vertex_buffer;
        HelloTriangle::Buffer* _index_buffer;
        std::vector<HelloTriangle::Buffer> _uniform_buffer;

        // MVP matrix
        struct MVPMatrix {
                glm::mat4 model;
                glm::mat4 view;
                glm::mat4 proj;
        };

        std::vector<HelloTriangle::vertex> _vertices;
        std::vector<uint32_t> _indices;
        MVPMatrix _mvp_matrix;

        //std::unique_ptr<>
};


