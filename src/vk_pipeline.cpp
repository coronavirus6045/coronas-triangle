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

//pipeline_state::pipeline_state(VkDevice& device_arg) : device(device_arg) {}
PipelineMaker::PipelineAllocator() {
    init();
}

void PipelineMaker::init() {
    //dont worry sType is always first
    rasterizer_info = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    color_blending_info = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    input_assembly_info = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    vertex_input_info = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    multisampling_info = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    viewport_info = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    pipeline_layout_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
}

PipelineHandle PipelineMaker::create_graphics_pipeline(GraphicsPipelineInfo info, RenderPass render_pass, std::vector<DescriptorLayout> layouts) {
    VkPipelineViewportStateCreateInfo viewport_info{};
    viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount = info.viewport_count;
    viewport_info.scissorCount = info.scissor_count;

    std::vector<VkDynamicState> states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_state_info{};
    dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = states.size();
    dynamic_state_info.pDynamicStates = states.data();

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = shader_stage_infos.size();
    pipeline_info.pStages = shader_stage_infos.data();
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly_info;
    pipeline_info.pViewportState = &viewport_info;
    pipeline_info.pRasterizationState = &rasterizer_info;
    pipeline_info.pMultisampleState = &multisampling_info;
    pipeline_info.pDepthStencilState = nullptr;
    pipeline_info.pColorBlendState = &color_blending_info;
    pipeline_info.pDynamicState = &dynamic_state_info;
    pipeline_info.layout = p_pipeline_layout;
    pipeline_info.renderPass = render_pass.get();
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;

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

    //descriptor_layouts.push_back(layouts);

    pipeline_layout_info.setLayoutCount = (uint32_t) descriptor_layouts.size();
    pipeline_layout_info.pSetLayouts = &descriptor_layouts.data()->get_layout();

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
    blend_info.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f};
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

void PipelineLayout::set_layouts(std::vector<DescriptorLayout> layouts) {
    for (uint32_t i = 0; i < layouts.size(); i++) {
        _layouts.push_back(layouts[i].get_layout());
    }
    //_layouts = layouts;
}

void PipelineLayout::create(VkPipelineLayoutCreateFlags flags) {
    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.flags = flags;
    layout_info.setLayoutCount = _layouts.size();
    layout_info.pSetLayouts = _layouts.data();
    CHECK_FOR_VK_RESULT(vkCreatePipelineLayout(, &layout_info, nullptr, &_pipeline_layout), "");
}

Subpass::Subpass() {
    _subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
}

void Subpass::set_color_attachment_reference(uint32_t attachment_index) {
    VkAttachmentReference color_reference{};
    color_reference.attachment = attachment_index;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    _color_references.push_back(color_reference);

    _subpass.colorAttachmentCount = (uint32_t) _color_references.size();
    _subpass.pColorAttachments = _color_references.data();
}

void RenderPass::create(std::vector<Subpass> subpasses) {
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = (uint32_t) _attachments.size();
    render_pass_info.pAttachments = _attachments.data();
    render_pass_info.subpassCount = 1;

    render_pass_info.pSubpasses = _subpasses.data();
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;
}

void RenderPass::set_attachment(RenderAttachment attachment) {
    VkAttachmentDescription _attachment {};
    _attachment.format = attachment.format;
    _attachment.samples = attachment.samples;
    _attachment.loadOp = attachment.load_op;
    _attachment.storeOp = attachment.store_op;
    _attachment.stencilLoadOp = attachment.stencil_load_op;
    _attachment.stencilStoreOp = attachment.stencil_store_op;
    _attachment.initialLayout = attachment.initial_layout;
    _attachment.finalLayout = attachment.final_layout;

    _attachments.push_back(_attachment);
}


PipelineMaker::PipelineAllocator() {
       // auto vertShaderCode = HelloTriangle::readFile("vert.spv");
        //auto fragShaderCode = HelloTriangle::readFile("frag.spv");

        //VkShaderModule vertShaderModule = create_shader_module(vertShaderCode);
        //VkShaderModule fragShaderModule = create_shader_module(fragShaderCode);
        
        auto bindingDescription = vertex::get_binding_description();
        auto attributeDescriptions = vertex::get_attribute_description();
        /*
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
%
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
    */
        //VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        /*VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
    */
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        //consult if true
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
        
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Joever error: Can't make pipeline layout");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;
        
        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("Can't cook error: Bro, i can't make a pipeline.");
        }

        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

RenderPass::RenderPass(VkFormat swapChainImageFormat) {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        //how do we do multiple subpasses, well we do a separate subpass class. If i went to 1.3, all this SHIT will be done
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pInputAttachments = nullptr;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("Joever error: Can't make the render pass.");
        }
}

VkShaderModule pipeline_state::create_shader_module(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;

        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Joever error: Can't make shader module, womp womp.");
        }
        return shaderModule;
}


pipeline_state::~pipeline_state() {
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    
}
