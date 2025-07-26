#include "renderer.hpp"
#include <thread>

#define IMGUI_IMPL_VULKAN_USE_VOLK


void Renderer::boot() {
    SDL_Event event;

    while (!feel_like_it_wants_to_exit) {
        while (SDL_PollEvent(&event) != 0) {
            ImGui_ImplSDL2_ProcessEvent(&event);
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

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();

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

    // Main pipeline maker (Gone forever :))
    
    //_pipeline_maker = new HelloTriangle::PipelineMaker(*_device);
    
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

    HelloTriangle::Shader main_vert(*_device, "main_vert.spv", "main");
    HelloTriangle::Shader main_frag(*_device, "main_frag.spv", "main");


    std::vector<HelloTriangle::RenderAttachmentReference> attachment_refs;

    HelloTriangle::RenderAttachmentReference color_ref;
    color_ref.attachment_index = 0;
    color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachment_refs.push_back(color_ref);


    HelloTriangle::RenderAttachmentReference depth_ref;
    depth_ref.attachment_index = 1;
    depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    std::vector<HelloTriangle::Subpass> subpasses;

    HelloTriangle::Subpass subpass;
    subpass.color_references = attachment_refs;
    subpass.depth_reference = depth_ref;
    subpass.bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpasses.push_back(subpass);

    //std::vector<HelloTriangle::RenderAttachment> attachments;

    HelloTriangle::RenderAttachment color_attachment{};
    color_attachment.format = _swapchain->get_format();
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.load_op = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.store_op = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencil_load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencil_store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    HelloTriangle::RenderAttachment depth_attachment{};
    depth_attachment.format = VK_FORMAT_D32_SFLOAT;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.load_op = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencil_load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencil_store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    std::vector<HelloTriangle::RenderAttachment> attachments = {color_attachment, depth_attachment};

    std::vector<HelloTriangle::SubpassDependency> deps;

    HelloTriangle::SubpassDependency dependency{};
    dependency.src_subpass = VK_SUBPASS_EXTERNAL;
    dependency.dst_subpass = 0;
    dependency.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.src_access_mask = 0;
    dependency.dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dst_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    deps.push_back(dependency);

    _main_pass = new HelloTriangle::RenderPass(*_device, subpasses, attachments, deps);

    // Vertex buffer

    _vertices = {
                 {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                 {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                 {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                 {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

                 {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                 {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                 {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                 {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };

    // is this stupid?
    std::cout << "VERTEX BUFFER\n";

    //HelloTriangle::Buffer vbuf = HelloTriangle::create_vertex_buffer(*_device, _command_pool->get(), _vertices);
    _vertex_buffer = new HelloTriangle::Buffer(HelloTriangle::create_vertex_buffer(*_device, *_command_pool, _vertices));


    // Index buffer
    std::cout << "VERTEX BUFFER\n";

    _indices = {
                0, 1, 2, 2, 3, 0,
                4, 5, 6, 6, 7, 4
    };
    
    //HelloTriangle::Buffer ibuf = HelloTriangle::create_index_buffer(*_device, _command_pool->get(), _indices);
    _index_buffer = new HelloTriangle::Buffer(HelloTriangle::create_index_buffer(*_device, *_command_pool, _indices));

    // Uniform buffer
    std::cout << "UNIFORM BUFFER\n";

    _mvp_matrix = {};

    _uniform_buffer.resize(MAX_FRAMES_IN_FLIGHT);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //the sisyphean task
        //HelloTriangle::Buffer ubuf(*_device, sizeof(MVPMatrix),VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
        //_uniform_buffer.push_back(std::move(ubuf));

        // COMMENT 168 AND 169 AND UNCOMMENT BELOW AND LINE 165 TO NOT TRIGGER THE COPY CONSTRUCTOR OF VECTOR
        _uniform_buffer[i].create(*_device, sizeof(MVPMatrix),VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
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

    // Image
    _picture = std::make_shared<HelloTriangle::Image>(HelloTriangle::Image(load_image_from_file("test.png")));
    // do we need a sampler for depth image?
    _depth_image = std::make_shared<HelloTriangle::Image>(HelloTriangle::Image(*_device, _swapchain->get_image_width(), _swapchain->get_image_height(), VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 0, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE));

    _descriptor_layout = new HelloTriangle::DescriptorLayout();

    std::vector<HelloTriangle::DescriptorBinding> bindings;

    HelloTriangle::DescriptorBinding uniform_bind{};
    uniform_bind.binding = 0;
    uniform_bind.descriptor_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniform_bind.descriptor_count = 1;
    uniform_bind.stage_flags = VK_SHADER_STAGE_VERTEX_BIT;

    HelloTriangle::DescriptorBinding image_bind{};
    image_bind.binding = 1;
    image_bind.descriptor_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    image_bind.descriptor_count = 1;
    image_bind.stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT;

    bindings.push_back(uniform_bind);
    bindings.push_back(image_bind);

    //_descriptor_layout->add_binding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
   // _descriptor_layout->add_binding(1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_VERTEX_BIT);
    _descriptor_layout->create(*_device, bindings);

    _descriptor_pool = new HelloTriangle::DescriptorPool(*_device, 2, bindings);

    _descriptor_set.resize(MAX_FRAMES_IN_FLIGHT);
    std::cout << "DESCRIPTOR SET\n";

    //std::vector<HelloTriangle::DescriptorLayout> desc_layouts(MAX_FRAMES_IN_FLIGHT, *_descriptor_layout);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //HelloTriangle::DescriptorSet set;
        _descriptor_set[i].allocate(*_device, *_descriptor_pool, *_descriptor_layout);
        _descriptor_set[i].write_descriptor(uniform_bind, &_uniform_buffer[i], 0, sizeof(MVPMatrix), nullptr);
        //_descriptor_set.push_back(set);
        _descriptor_set[i].write_descriptor(image_bind, nullptr, 0, 0, _picture.get());
    }

    //Add constructor with no args
    _pipeline_layout = new HelloTriangle::PipelineLayout(*_device, *_descriptor_layout, 0);
    
    //_pipeline_maker->set_shader(main_vert, VK_SHADER_STAGE_VERTEX_BIT, "main");
    //_pipeline_maker->set_shader(main_frag, VK_SHADER_STAGE_FRAGMENT_BIT, "main");
    
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

    std::vector<HelloTriangle::VertexAttribute> vert_attributes(3);

    vert_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vert_attributes[0].location = 0;
    vert_attributes[0].offset = offsetof(HelloTriangle::vertex, pos);

    vert_attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vert_attributes[1].location = 1;
    vert_attributes[1].offset = offsetof(HelloTriangle::vertex, color);

    vert_attributes[2].format = VK_FORMAT_R32G32_SFLOAT;
    vert_attributes[2].location = 2;
    vert_attributes[2].offset = offsetof(HelloTriangle::vertex, tex_coord);

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
    pipeline_main_info.enable_depth_test = true;
    pipeline_main_info.enable_depth_write = true;
    pipeline_main_info.depth_compare_op = VK_COMPARE_OP_LESS;

        //_pipelines[0] = _pipeline_maker->create_graphics_pipeline(pipeline_main_info, *_pipeline_layout, *_main_pass);
    _pipeline_main = new HelloTriangle::GraphicsPipeline(*_device, pipeline_main_info, *_pipeline_layout, main_vert, main_frag, *_main_pass);

    uint32_t swapchain_size = _swapchain->get_images().size();


    _main_framebuffer.resize(swapchain_size);
    _swapchain_images.resize(swapchain_size);

    for (uint32_t i = 0; i < swapchain_size; i++) {
        _swapchain_images[i] = std::make_shared<HelloTriangle::Image>(HelloTriangle::Image(*_swapchain, i));

        std::vector<std::shared_ptr<HelloTriangle::Image>> images = {_swapchain_images[i], _depth_image};

        _main_framebuffer[i] = HelloTriangle::Framebuffer();
        _main_framebuffer[i].create(*_device, *_main_pass, images);
    }

    // Synchronization
    _sync_object_maker = new HelloTriangle::SyncObjectMaker(*_device);

    _render_semaphore.resize(_swapchain_images.size());
    _image_semaphore.resize(MAX_FRAMES_IN_FLIGHT);
    _main_fence.resize(MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        _image_semaphore[i] = _sync_object_maker->create_semaphore();
        _main_fence[i] = _sync_object_maker->create_fence();
    }
    for (uint32_t i = 0; i < _swapchain_images.size(); i++) {
        _render_semaphore[i] = _sync_object_maker->create_semaphore();
    }
    //_uniform_buffer.clear();
        setup_imgui();
}

void Renderer::draw() {
    vkWaitForFences(_device->get_device(), 1, &_main_fence[_frame_index], VK_TRUE, UINT64_MAX);

    VkResult result = _swapchain->acquire_next_image(_image_semaphore[_frame_index], nullptr);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        std::cout << "PP\n";
        swapchain_resize();
    }

    uint32_t image_index = _swapchain->get_image_index();

    // swapchain recreation
    _main_command_buffer[_frame_index].reset();

    _main_command_buffer[_frame_index].begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    vkResetFences(_device->get_device(), 1, &_main_fence[_frame_index]);
    VkExtent2D extent = {_swapchain->get_image_width(), _swapchain->get_image_height()};

    VkRenderPassBeginInfo pass_info{};
    pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass_info.renderPass = _main_pass->get();
    //culprit
    pass_info.framebuffer = _main_framebuffer[image_index].get();
    pass_info.renderArea.offset = {0, 0};
    pass_info.renderArea.extent = extent;

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = {0.0f, 0.0f, 0.0f, 0.0f};
    clear_values[1].depthStencil = {1.0f, 0};

    pass_info.clearValueCount = clear_values.size();
    pass_info.pClearValues = clear_values.data();

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

    vkCmdBindPipeline(_main_command_buffer[_frame_index].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_main->get());

    vkCmdBindDescriptorSets(_main_command_buffer[_frame_index].get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_layout->get(), 0, 1, &_descriptor_set[_frame_index].get(), 0, nullptr);

    vkCmdDrawIndexed(_main_command_buffer[_frame_index].get(), _indices.size(), 1, 0, 0, 0);

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _main_command_buffer[_frame_index].get());

    vkCmdEndRenderPass(_main_command_buffer[_frame_index].get());

    _main_command_buffer[_frame_index].end();

    uniform_update(extent);

    std::vector<HelloTriangle::Semaphore> wait = {_image_semaphore[_frame_index]};
    std::vector<HelloTriangle::Semaphore> signal = {_render_semaphore[image_index]};

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

HelloTriangle::Image Renderer::load_image_from_file(std::string path) {
    int32_t image_width = 0;
    int32_t image_height = 0;
    int32_t image_channels = 0;

    stbi_uc* pixels = stbi_load(path.c_str(), &image_width, &image_height, &image_channels, STBI_rgb_alpha);

    VkDeviceSize image_size = image_width * image_height * 4;

    HelloTriangle::Image image;

    if (!pixels) {
        THROW_RUNTIME_ERROR("Image texture error.");
    }

    HelloTriangle::Buffer staging_buffer(*_device, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
    staging_buffer.map_memory(pixels, image_size, 0);
    staging_buffer.unmap_memory();

    stbi_image_free(pixels);

    image.create(*_device, image_width, image_height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 0, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

    image.transition_image_layout(*_command_pool, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    image.copy_buffer_to_image(*_command_pool, staging_buffer, image_width, image_height);
    image.transition_image_layout(*_command_pool, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    // thanks to destructors, staging buffer is bye byee
    return image;
}

void Renderer::setup_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForVulkan(_window);
    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = _instance->get();
    init_info.PhysicalDevice = _device->get_physical_device();
    init_info.Device = _device->get_device();
    init_info.QueueFamily = _device->get_queue_family();
    init_info.Queue = _device->get_graphics_queue();
    init_info.PipelineCache = nullptr;
    init_info.DescriptorPool = _descriptor_pool->get();
    init_info.RenderPass = _main_pass->get();
    init_info.Subpass = 0;
    init_info.MinImageCount = 2;
    init_info.ImageCount = 2;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    //init_info.CheckVkResultFn = [] (VkResult res) { if (res != VK_SUCCESS) {THROW_RUNTIME_ERROR("ImGui had a problem.")} };

    //ImGui_ImplVulkan_LoadFunctions(VK_API_VERSION_1_2, [](const char* function_name, void* vk_instance) { return vkGetInstanceProcAddr((VkInstance) vk_instance, function_name); }, _instance->get());
    ImGui_ImplVulkan_Init(&init_info);
}

void Renderer::cleanup() {
    //this is when you will use smart pointers

    delete _swapchain;

    //lets go back to turning pipeline as a single class instead of handle and maker
    //_pipeline_maker->delete_pipeline(_pipelines[0]);

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
    //delete _main_framebuffer;

    delete _vertex_buffer;
    delete _index_buffer;

    _uniform_buffer.clear();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    delete _device;
    delete _instance;
}
