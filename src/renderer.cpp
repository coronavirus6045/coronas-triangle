#include "renderer.hpp"
#include <thread>

void Renderer::boot() {
    SDL_Event event;

    while (!feel_like_it_wants_to_exit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                feel_like_it_wants_to_exit = true;
            }

            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    feel_like_it_wants_to_stop_rendering = true;
                } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    feel_like_it_wants_to_stop_rendering = false;
                }
            }
        }

        if (feel_like_it_wants_to_stop_rendering) {
            std::cout << "SLEEP\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        draw();
    }

    cleanup();
}

void Renderer::initialize() {
    // SDL window
    
    _window = SDL_CreateWindow("vktest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 800, (SDL_WindowFlags) SDL_WINDOW_VULKAN);
    
    // Instance
    
    std::vector<const char*> ext;
    uint32_t ext_count = 0;
    SDL_Vulkan_GetInstanceExtensions(_window, &ext_count, nullptr);
    ext.resize(ext_count);
    SDL_Vulkan_GetInstanceExtensions(_window, &ext_count, ext.data());


    _instance = new HelloTriangle::Instance("hello", ext);

    
    // Surface
    
    SDL_Vulkan_CreateSurface(_window, _instance->get(), &_surface);
    
    // Device

    _device = new HelloTriangle::Device(*_instance, 0, _surface);
    
    // Swapchain
    
    _swapchain = new HelloTriangle::Swapchain(*_device, _surface, VK_FORMAT_B8G8R8A8_SRGB, VK_PRESENT_MODE_FIFO_KHR, window_size_to_extent());

    // Main pipeline maker
    
    _pipeline_maker = new HelloTriangle::PipelineMaker(*_device);
    
    // Command pool
    
    _command_pool = new HelloTriangle::CommandPool(*_device);
    
    // Command buffers
    
    _main_command_buffer.resize(MAX_FRAMES_IN_FLIGHT);

    for (auto& i : _main_command_buffer) {
        i.create(*_device, *_command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    }
    
    // Main pipeline handle
    
    HelloTriangle::PipelineHandle pipeline_main;
    _pipelines.push_back(pipeline_main);
    
    // Main shaders (should be in a separate functi)

    HelloTriangle::Shader main_vert(*_device, "main_vert.spv");
    HelloTriangle::Shader main_frag(*_device, "main_frag.spv");


    std::vector<HelloTriangle::RenderAttachmentReference> attachment_refs;

    HelloTriangle::RenderAttachmentReference color_ref;
    color_ref.attachment_index = 0;
    color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachment_refs.push_back(color_ref);

    std::vector<HelloTriangle::Subpass> subpasses;

    HelloTriangle::Subpass subpass;
    subpass.color_references = attachment_refs;
    subpass.bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpasses.push_back(subpass);

    std::vector<HelloTriangle::RenderAttachment> attachments;

    HelloTriangle::RenderAttachment color_attachment;
    color_attachment.format = _swapchain->get_format();
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.load_op = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.store_op = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencil_load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencil_store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachments.push_back(color_attachment);

    std::vector<HelloTriangle::SubpassDependency> deps;

    HelloTriangle::SubpassDependency dependency{};
    dependency.src_subpass = VK_SUBPASS_EXTERNAL;
    dependency.dst_subpass = 0;
    dependency.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.src_access_mask = 0;
    dependency.dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dst_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    deps.push_back(dependency);

    _main_pass = new HelloTriangle::RenderPass(*_device, subpasses, attachments, deps);

    // Vertex buffer

    _vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
    };

    // is this stupid?
    std::cout << "VERTEX BUFFER\n";

    //HelloTriangle::Buffer vbuf = HelloTriangle::create_vertex_buffer(*_device, _command_pool->get(), _vertices);
    _vertex_buffer = new HelloTriangle::Buffer(HelloTriangle::create_vertex_buffer(*_device, *_command_pool, _vertices));


    // Index buffer
    std::cout << "VERTEX BUFFER\n";

    _indices = {
        0, 1, 2, 2, 3, 0
    };
    
    //HelloTriangle::Buffer ibuf = HelloTriangle::create_index_buffer(*_device, _command_pool->get(), _indices);
    _index_buffer = new HelloTriangle::Buffer(HelloTriangle::create_index_buffer(*_device, *_command_pool, _indices));

    // Uniform buffer
    std::cout << "UNIFORM BUFFER\n";

    _mvp_matrix = {};

    _uniform_buffer.resize(MAX_FRAMES_IN_FLIGHT);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //HelloTriangle::Buffer ubuf = HelloTriangle::create_uniform_buffer(*_device, &_mvp_matrix, sizeof(MVPMatrix));
        //_uniform_buffer[i] = HelloTriangle::create_uniform_buffer(*_device, &_mvp_matrix, sizeof(MVPMatrix));
        //_uniform_buffer[i] = ubuf;
        _uniform_buffer[i].create(*_device, sizeof(MVPMatrix),VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        _uniform_buffer[i].map_memory(&_mvp_matrix, sizeof(MVPMatrix), 0, 0);
    }
    // Fix constructor
    //_descriptor_layout.resize(MAX_FRAMES_IN_FLIGHT);

    /*
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //HelloTriangle::DescriptorLayout layout(*_device);
        _descriptor_layout[i].add_binding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);
        _descriptor_layout[i].create(*_device);
        //rule of 3 violations and realization desctructor is called after pushback
        //_descriptor_layout.push_back(layout);
        //_descriptor_layout.emplace_back(layout);
    }
*/
    //

    _descriptor_layout = new HelloTriangle::DescriptorLayout();
    _descriptor_layout->add_binding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    _descriptor_layout->create(*_device);

    _descriptor_pool = new HelloTriangle::DescriptorPool(*_device, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

    _descriptor_set.resize(MAX_FRAMES_IN_FLIGHT);
    std::cout << "DESCRIPTOR SET\n";

    //std::vector<HelloTriangle::DescriptorLayout> desc_layouts(MAX_FRAMES_IN_FLIGHT, *_descriptor_layout);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //HelloTriangle::DescriptorSet set;
        _descriptor_set[i].allocate(*_device, *_descriptor_pool, *_descriptor_layout);
        _descriptor_set[i].write_descriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &_uniform_buffer[i], 0, sizeof(MVPMatrix), nullptr);
        //_descriptor_set.push_back(set);
    }

    //Add constructor with no args
    _pipeline_layout = new HelloTriangle::PipelineLayout(*_device, *_descriptor_layout, 0);
    
    _pipeline_maker->set_shader(main_vert, VK_SHADER_STAGE_VERTEX_BIT, "main");
    _pipeline_maker->set_shader(main_frag, VK_SHADER_STAGE_FRAGMENT_BIT, "main");
    
    //Color blend attachment
    
    std::vector<HelloTriangle::ColorBlendAttachment> blend_attachments;

    HelloTriangle::ColorBlendAttachment blend_attachment{};
    blend_attachment.enable_blend = false;
    blend_attachment.color_write = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    blend_attachments.push_back(blend_attachment);

    HelloTriangle::VertexFormat vert_format{};
    vert_format.binding = 0;
    vert_format.input_rate = VK_VERTEX_INPUT_RATE_VERTEX;
    vert_format.stride = sizeof(HelloTriangle::vertex);

    std::vector<HelloTriangle::VertexAttribute> vert_attributes(2);

    vert_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vert_attributes[0].location = 0;
    vert_attributes[0].offset = offsetof(HelloTriangle::vertex, pos);

    vert_attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vert_attributes[1].location = 1;
    vert_attributes[1].offset = offsetof(HelloTriangle::vertex, color);

    vert_format.attributes = vert_attributes;

    std::vector<HelloTriangle::VertexFormat> vert_formats;
    vert_formats.push_back(vert_format);

    HelloTriangle::GraphicsPipelineInfo pipeline_main_info{};
    pipeline_main_info.viewport_count = 1;
    pipeline_main_info.scissor_count = 1;
    pipeline_main_info.enable_primitive_restart = false;
    pipeline_main_info.enable_rasterizer_discard = false;
    pipeline_main_info.rasterizer_polygon_mode = VK_POLYGON_MODE_FILL;
    pipeline_main_info.rasterizer_line_width = 1.0f;
    pipeline_main_info.rasterizer_front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pipeline_main_info.rasterizer_cull_mode = VK_CULL_MODE_BACK_BIT;
    pipeline_main_info.subpass_index = 0;
    pipeline_main_info.enable_multisampling = false;
    pipeline_main_info.enable_rasterizer_depth_clamp = false;
    pipeline_main_info.vertex_input_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipeline_main_info.attachments = blend_attachments;
    pipeline_main_info.multisampling_sample_count = VK_SAMPLE_COUNT_1_BIT;
    pipeline_main_info.enable_color_blending_logic_op = false;
    pipeline_main_info.vertex_formats = vert_formats;


    _pipelines[0] = _pipeline_maker->create_graphics_pipeline(pipeline_main_info, *_pipeline_layout, *_main_pass);

    //_main_framebuffer.resize(_swapchain_size);

    //for (uint32_t i = 0; i < _swapchain_size; i++) {
    _main_framebuffer = new HelloTriangle::Framebuffer();
    _main_framebuffer->create(*_device, *_main_pass, *_swapchain);
    //}

    // Synchronization
    _sync_object_maker = new HelloTriangle::SyncObjectMaker(*_device);

    _render_semaphore.resize(MAX_FRAMES_IN_FLIGHT);
    _image_semaphore.resize(MAX_FRAMES_IN_FLIGHT);
    _main_fence.resize(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        _render_semaphore[i] = _sync_object_maker->create_semaphore();
        _image_semaphore[i] = _sync_object_maker->create_semaphore();
        _main_fence[i] = _sync_object_maker->create_fence();
    }

    //_uniform_buffer.clear();

}

void Renderer::draw() {
    vkWaitForFences(_device->get_device(), 1, &_main_fence[_frame_index], VK_TRUE, UINT64_MAX);

    VkResult result = _swapchain->acquire_next_image(_image_semaphore[_frame_index], nullptr);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        std::cout << "PP\n";
        swapchain_resize();
    }
    // swapchain recreation
    _main_command_buffer[_frame_index].reset();

    _main_command_buffer[_frame_index].begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    vkResetFences(_device->get_device(), 1, &_main_fence[_frame_index]);
    VkExtent2D extent = {_swapchain->get_image_width(), _swapchain->get_image_height()};

    VkRenderPassBeginInfo pass_info{};
    pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass_info.renderPass = _main_pass->get();
    //culprit
    pass_info.framebuffer = _main_framebuffer->get(_swapchain->get_image_index());
    pass_info.renderArea.offset = {0, 0};
    pass_info.renderArea.extent = extent;

    VkClearValue clear_color{{{0.0f, 0.0f, 0.0f, 0.0f}}};

    pass_info.clearValueCount = 1;
    pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(_main_command_buffer[_frame_index].get(), &pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.width = _swapchain->get_image_width();
    viewport.height = _swapchain->get_image_height();
    viewport.x = 0.0f;
    viewport.y = 0.0f;

    vkCmdSetViewport(_main_command_buffer[_frame_index].get(), 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.extent = extent;
    scissor.offset = {0, 0};

    vkCmdSetScissor(_main_command_buffer[_frame_index].get(), 0, 1, &scissor);

    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(_main_command_buffer[_frame_index].get(), 0, 1, &_vertex_buffer->buffer(), offsets);
    vkCmdBindIndexBuffer(_main_command_buffer[_frame_index].get(), _index_buffer->buffer(), 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindPipeline(_main_command_buffer[_frame_index].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, (VkPipeline) _pipelines[0]);

    vkCmdBindDescriptorSets(_main_command_buffer[_frame_index].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_layout->get(), 0, 1, &_descriptor_set[_frame_index].get(), 0, nullptr);

    vkCmdDrawIndexed(_main_command_buffer[_frame_index].get(), _indices.size(), 1, 0, 0, 0);

    vkCmdEndRenderPass(_main_command_buffer[_frame_index].get());

    _main_command_buffer[_frame_index].end();

    uniform_update(extent);

    std::vector<HelloTriangle::Semaphore> wait = {_image_semaphore[_frame_index]};
    std::vector<HelloTriangle::Semaphore> signal = {_render_semaphore[_frame_index]};

    VkPipelineStageFlags wait_dst_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    _main_command_buffer[_frame_index].submit(&wait, &signal, _main_fence[_frame_index], &wait_dst_stage);


    result = _swapchain->present(signal);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        swapchain_resize();
    }

    _frame_index = (_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::uniform_update(VkExtent2D extent) {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto current_time = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

    MVPMatrix mat{};
    mat.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mat.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mat.proj = glm::perspective(glm::radians(45.0f), extent.width / (float) extent.height, 0.0f, 10.0f);
    mat.proj[1][1] *= -1;
    _uniform_buffer[_frame_index].update_memory_map(&mat, sizeof(mat));
}

void Renderer::swapchain_resize() {
    delete _swapchain;
    _swapchain = new HelloTriangle::Swapchain(*_device, _surface, VK_FORMAT_B8G8R8_SRGB, VK_PRESENT_MODE_FIFO_KHR, window_size_to_extent());
}

VkExtent2D Renderer::window_size_to_extent() {
    int32_t width, height;
    SDL_GetWindowSize(_window, &width, &height);
    VkExtent2D extent = {(uint32_t) width, (uint32_t) height};
    return extent;
}

void Renderer::cleanup() {
    //this is when you will use smart pointers

    delete _swapchain;

    //lets go back to turning pipeline as a single class instead of handle and maker
    _pipeline_maker->delete_pipeline(_pipelines[0]);

    //delete _pipeline_maker;

    delete _command_pool;
    _main_command_buffer.clear();
    delete _descriptor_layout;
    delete _descriptor_pool;
    _descriptor_set.clear();

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        _sync_object_maker->delete_sync_object(_main_fence[i], nullptr);
        _sync_object_maker->delete_sync_object(nullptr, _image_semaphore[i]);
        _sync_object_maker->delete_sync_object(nullptr, _render_semaphore[i]);
    }
    //delete _sync_object_maker;

    delete _main_pass;
    delete _main_framebuffer;

    delete _vertex_buffer;
    delete _index_buffer;

    _uniform_buffer.clear();

    delete _device;
    delete _instance;
}
