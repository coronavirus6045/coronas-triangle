#include "vk_pipeline.hpp"
#include "load_file.hpp"
#include "scene/scene_obj.hpp"
#include "vk_shader.hpp"
#include <cstdint>
using HelloTriangle::PipelineMaker;
using HelloTriangle::PipelineHandle;
using HelloTriangle::RenderPass;
using HelloTriangle::Subpass;
using HelloTriangle::RenderAttachment;
using HelloTriangle::PipelineLayout;
using HelloTriangle::Framebuffer;

//pipeline_state::pipeline_state(VkDevice& device_arg) : device(device_arg) {}
PipelineMaker::PipelineMaker(Device& device) : _device(device) {}

PipelineHandle PipelineMaker::create_graphics_pipeline(GraphicsPipelineInfo info, PipelineLayout& layout, RenderPass& render_pass, std::vector<DescriptorLayout> layouts) {
    VkPipelineViewportStateCreateInfo viewport_info{};
    viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount = info.viewport_count;
    viewport_info.scissorCount = info.scissor_count;

    std::array<VkDynamicState, 2> states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_state_info{};
    dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = states.size();
    dynamic_state_info.pDynamicStates = states.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
    input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology = info.vertex_input_topology;
    input_assembly_info.primitiveRestartEnable = (VkBool32) info.enable_primitive_restart; //wtf is this

    VkPipelineRasterizationStateCreateInfo rasterizer_info{};
    rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer_info.cullMode = info.rasterizer_cull_mode;
    rasterizer_info.frontFace = info.rasterizer_front_face;
    rasterizer_info.polygonMode = info.rasterizer_polygon_mode;
    rasterizer_info.lineWidth = info.rasterizer_line_width;
    rasterizer_info.rasterizerDiscardEnable = info.enable_rasterizer_discard;
    rasterizer_info.depthClampEnable = info.enable_rasterizer_depth_clamp;
    rasterizer_info.depthBiasEnable = info.enable_rasterizer_depth_bias;
    rasterizer_info.depthBiasConstantFactor = info.rasterizer_depth_bias_constant;
    rasterizer_info.depthBiasClamp = info.rasterizer_depth_bias_clamp;
    rasterizer_info.depthBiasSlopeFactor = info.rasterizer_depth_bias_slope;

    VkPipelineMultisampleStateCreateInfo multisampling_info{};
    multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_info.sampleShadingEnable = (VkBool32) info.enable_multisampling;
    multisampling_info.rasterizationSamples = info.multisampling_sample_count;

    std::vector<VkPipelineColorBlendAttachmentState> attachments(info.attachments.size());

    for (uint32_t i = 0; i < info.attachments.size(); i++) {
        attachments[i].blendEnable = (VkBool32) info.attachments[i].enable_blend;
        attachments[i].colorBlendOp = info.attachments[i].color_blend_op;
        attachments[i].srcColorBlendFactor = info.attachments[i].src_color_blend_factor;
        attachments[i].dstColorBlendFactor = info.attachments[i].dst_color_blend_factor;
        attachments[i].alphaBlendOp = info.attachments[i].alpha_blend_op;
        attachments[i].srcAlphaBlendFactor = info.attachments[i].src_alpha_blend_factor;
        attachments[i].dstAlphaBlendFactor = info.attachments[i].dst_alpha_blend_factor;
        attachments[i].colorWriteMask = info.attachments[i].color_write;
    }

    VkPipelineColorBlendStateCreateInfo blend_info{};
    blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend_info.logicOpEnable = info.enable_color_blending_logic_op;
    blend_info.logicOp = info.color_blending_logic_op;
    blend_info.flags = info.color_blending_flags;
    blend_info.attachmentCount = info.attachments.size();
    blend_info.pAttachments = attachments.data();
    //blend_info.blendConstants = ;
    //leave it all to 0.0f

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = _shader_stage_infos.size();
    pipeline_info.pStages = _shader_stage_infos.data();
    pipeline_info.pVertexInputState = nullptr;
    pipeline_info.pInputAssemblyState = &input_assembly_info;
    pipeline_info.pViewportState = &viewport_info;
    pipeline_info.pRasterizationState = &rasterizer_info;
    pipeline_info.pMultisampleState = &multisampling_info;
    pipeline_info.pDepthStencilState = nullptr;
    pipeline_info.pColorBlendState = &blend_info;
    pipeline_info.pDynamicState = &dynamic_state_info;
    pipeline_info.layout = layout.get();
    pipeline_info.renderPass = render_pass.get();
    pipeline_info.subpass = info.subpass_index;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;

    VkPipeline pipeline;
    //VkPipelineDepthStencilStateCreateInfo
    CHECK_FOR_VK_RESULT(vkCreateGraphicsPipelines(_device.get_device(), nullptr, 1, &pipeline_info, nullptr, &pipeline), "");
    _shader_stage_infos.clear();
    return (PipelineHandle) pipeline;
}

void PipelineMaker::set_shader(HelloTriangle::Shader& shader, VkShaderStageFlagBits flags, std::string name) {
    VkPipelineShaderStageCreateInfo shader_stage_info{};
    shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_info.stage = flags;
    shader_stage_info.module = shader.get();
    shader_stage_info.pName = name.c_str();
    _shader_stage_infos.push_back(shader_stage_info);
}

void PipelineMaker::delete_pipeline(PipelineHandle pipeline) {
    vkDestroyPipeline(_device.get_device(), (VkPipeline) pipeline, nullptr);
}

PipelineLayout::PipelineLayout(Device& device, std::vector<DescriptorLayout> layouts, VkPipelineLayoutCreateFlags flags) : _device(device) {
    set_layouts(layouts);
    create(device, flags);
}

void PipelineLayout::set_layouts(std::vector<DescriptorLayout> layouts) {
    for (uint32_t i = 0; i < layouts.size(); i++) {
        _layouts.push_back(layouts[i].get_layout());
    }
}

void PipelineLayout::create(Device& device, VkPipelineLayoutCreateFlags flags) {
    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.flags = flags;
    layout_info.setLayoutCount = (uint32_t) _layouts.size();
    layout_info.pSetLayouts = _layouts.data();
    CHECK_FOR_VK_RESULT(vkCreatePipelineLayout(device.get_device(), &layout_info, nullptr, &_pipeline_layout), "");
    _device = device;
}

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(_device.get_device(), _pipeline_layout, nullptr);
}

VkAttachmentReference RenderPass::_create_reference(RenderAttachmentReference ref) {
    VkAttachmentReference _ref{};
    _ref.attachment = ref.attachment_index;
    _ref.layout = ref.layout;
    return _ref;
}

RenderPass::RenderPass(Device& device, std::vector<Subpass> subpasses, std::vector<RenderAttachment> attachments, std::vector<SubpassDependency> dependencies) : _device(device) {
    create(device, subpasses, attachments, dependencies);
}

void RenderPass::create(Device& device, std::vector<Subpass> subpasses, std::vector<RenderAttachment> attachments, std::vector<SubpassDependency> dependencies) {
    //std::vector<VkAttachmentReference> references;
    std::vector<VkAttachmentDescription> _attachments;

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = (uint32_t) _attachments.size();
    render_pass_info.pAttachments = _attachments.data();

    for (uint32_t i = 0; i < attachments.size(); i++) {
        _attachments[i].format = attachments[i].format;
        _attachments[i].samples = attachments[i].samples;
        _attachments[i].loadOp = attachments[i].load_op;
        _attachments[i].storeOp = attachments[i].store_op;
        _attachments[i].stencilLoadOp = attachments[i].stencil_load_op;
        _attachments[i].stencilStoreOp = attachments[i].stencil_store_op;
        _attachments[i].initialLayout = attachments[i].initial_layout;
        _attachments[i].finalLayout = attachments[i].final_layout;
    }
    //instead of manually making attachment refs, let's automatically create
    std::vector<VkSubpassDescription> _subpasses;

    for (uint32_t i = 0; i < subpasses.size(); i++) {
        _subpasses[i].pipelineBindPoint = subpasses[i].bind_point;

        //replace this with a macro that loops for all attachment types later,
        std::vector<VkAttachmentReference> _color_references(subpasses[i].color_references.size());
        //Get real
        for (uint32_t j = 0; j < _color_references.size(); j++) {
            _color_references[i] = _create_reference(subpasses[i].color_references[j]);
        }

        _subpasses[i].colorAttachmentCount = _color_references.size();
        _subpasses[i].pColorAttachments = _color_references.data();
    }

    std::vector<VkSubpassDependency> _dependencies(dependencies.size());

    for (uint32_t i = 0; i < dependencies.size(); i++) {
        _dependencies[i].srcSubpass = dependencies[i].src_subpass;
        _dependencies[i].dstSubpass = dependencies[i].dst_subpass;
        _dependencies[i].srcStageMask = dependencies[i].src_stage_mask;
        _dependencies[i].dstStageMask = dependencies[i].dst_stage_mask;
        _dependencies[i].srcAccessMask = dependencies[i].src_access_mask;
        _dependencies[i].dstAccessMask = dependencies[i].dst_access_mask;
    }


    render_pass_info.subpassCount = (uint32_t) subpasses.size();
    render_pass_info.pSubpasses = _subpasses.data();
    render_pass_info.dependencyCount = _dependencies.size();
    render_pass_info.pDependencies = _dependencies.data();

    CHECK_FOR_VK_RESULT(vkCreateRenderPass(device.get_device(), &render_pass_info, nullptr, &_render_pass), "");
    //This is in case if its created through this instead of the constructor. it looks kinda stupid but since its a reference, who cares?
    _device = device;
}

RenderPass::~RenderPass() {
    vkDestroyRenderPass(_device.get_device(), _render_pass, nullptr);
}

Framebuffer::Framebuffer(Device& device) : _device(device) {

}

Framebuffer::create(Device& device, RenderPass& pass, std::vector<coronas_triangle::image_creation::ImageTexture>) {
    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = pass.get();
    framebuffer_info.attachmentCount = ;
    framebuffer_info.pAttachments = ;
    framebuffer_info.width = ;
    framebuffer_info.height = ;
    framebuffer_info.layers = 1;

    CHECK_FOR_VK_RESULT(vkCreateFramebuffer(device.get_device(), &framebuffer_info, nullptr, &_framebuffer), "")
}
