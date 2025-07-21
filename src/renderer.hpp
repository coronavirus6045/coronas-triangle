#pragma once
#include "common.hpp"
#include "vk_device.hpp"
#include "vk_pipeline.hpp"
#include "vk_presentation.hpp"
#include "vk_syncobjects.hpp"
#include "vk_command.hpp"
#include "vk_instance.hpp"
#include "vk_buffers.hpp"
#include "vk_image.hpp"
#include "vk_shader.hpp"
#include "vk_descriptor.hpp"
#include "scene/scene_obj.hpp"


//We include vk_image later

//this is the master renderer class that will do all the work

//NEED TO BRING BACK VERTEX ATTRIBUTE AND5 BINDING DESC AND ALSO DO RULE OF 3

#define MAX_FRAMES_IN_FLIGHT 2

class Renderer {
    public:
        //Renderer();
        //~Renderer();
        void boot();
        void initialize();
        void draw();
        void swapchain_resize();
        void uniform_update(VkExtent2D extent);
        HelloTriangle::Image load_image_from_file(std::string path);
        void cleanup();
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
        std::vector<std::shared_ptr<HelloTriangle::Image>> _swapchain_images;

        // Main pipeline and synchronization maker
        HelloTriangle::GraphicsPipeline* _pipeline_main;
        HelloTriangle::SyncObjectMaker* _sync_object_maker;

        // Sync objects
        std::vector<HelloTriangle::Fence> _main_fence;
        std::vector<HelloTriangle::Semaphore> _image_semaphore;
        std::vector<HelloTriangle::Semaphore> _render_semaphore;

        // Main command_pool
        HelloTriangle::CommandPool* _command_pool;

        // Descriptor layout, pool, sets
        //std::vector<HelloTriangle::DescriptorLayout> _descriptor_layout;
        HelloTriangle::DescriptorLayout* _descriptor_layout;
        HelloTriangle::DescriptorPool* _descriptor_pool;
        HelloTriangle::PipelineLayout* _pipeline_layout;
        std::vector<HelloTriangle::DescriptorSet> _descriptor_set;

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

        // The Images
        std::shared_ptr<HelloTriangle::Image> _picture;
        std::shared_ptr<HelloTriangle::Image> _depth_image;

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


