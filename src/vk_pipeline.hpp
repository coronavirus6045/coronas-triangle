#pragma once

#include "common.hpp"
#include "vk_shader.hpp"
#include "vk_descriptor.hpp"
#include "vk_image.hpp"


namespace HelloTriangle {
    typedef VkPipeline PipelineHandle; //this feels stupid
    // we do it in a SINGLE struct
    //oh my FREAKING god i reinvented the struct LOL
    struct ColorBlendAttachment {
            bool enable_blend;
            VkBlendOp color_blend_op;
            VkBlendFactor src_color_blend_factor;
            VkBlendFactor dst_color_blend_factor;
            VkBlendOp alpha_blend_op;
            VkBlendFactor src_alpha_blend_factor;
            VkBlendFactor dst_alpha_blend_factor;
            VkColorComponentFlags color_write;
    };

    struct GraphicsPipelineInfo {
        //    std::map<Shader, VkShaderStageFlagBits> shader;
        uint32_t viewport_count;
        uint32_t scissor_count;
        VkPrimitiveTopology vertex_input_topology;
        bool enable_primitive_restart;
        bool enable_multisampling;
        VkSampleCountFlagBits multisampling_sample_count;
        bool enable_rasterizer_discard;
        VkPolygonMode rasterizer_polygon_mode;
        float rasterizer_line_width;
        VkCullModeFlags rasterizer_cull_mode;
        VkFrontFace rasterizer_front_face;
        bool enable_rasterizer_depth_clamp;
        bool enable_rasterizer_depth_bias;
        float rasterizer_depth_bias_constant;
        float rasterizer_depth_bias_clamp;
        float rasterizer_depth_bias_slope;
        VkPipelineColorBlendStateCreateFlags color_blending_flags;
        bool enable_color_blending_logic_op;
        VkLogicOp color_blending_logic_op;
        std::vector<ColorBlendAttachment> attachments;

        uint32_t subpass_index;

    };
    class PipelineLayout {
        public:
            PipelineLayout(Device& device, std::vector<DescriptorLayout> layouts, VkPipelineLayoutCreateFlags flags);
            ~PipelineLayout();
            void set_layouts(std::vector<DescriptorLayout> layouts);

            void create(Device& device, VkPipelineLayoutCreateFlags flags);
            VkPipelineLayout& get() {return _pipeline_layout;}

        private:
            VkPipelineLayout _pipeline_layout;
            std::vector<VkDescriptorSetLayout> _layouts;

            Device& _device;
    };

    //do we NEED this? ill maybe separate to PipelineInfo and its
    class RenderPass;
    //why
    class PipelineMaker {
    public:
    PipelineMaker(Device& device);
    ~PipelineMaker();
    //Resets
    //void init();
    void set_shader(HelloTriangle::Shader& shader, VkShaderStageFlagBits flags, std::string name);
    PipelineHandle create_graphics_pipeline(GraphicsPipelineInfo info, PipelineLayout& layout, RenderPass& render_pass, std::vector<DescriptorLayout> layouts);

    void delete_pipeline(PipelineHandle pipeline);
    private:
    std::vector<VkPipelineShaderStageCreateInfo> _shader_stage_infos;

    VkPipeline p_pipeline;

    Device& _device;
    };

    struct RenderAttachment {
            VkFormat format;
            VkSampleCountFlagBits samples;
            VkAttachmentLoadOp load_op;
            VkAttachmentStoreOp store_op;
            VkAttachmentLoadOp stencil_load_op;
            VkAttachmentStoreOp stencil_store_op;
            VkImageLayout initial_layout;
            VkImageLayout final_layout;
    };

    struct RenderAttachmentReference {
            uint32_t attachment_index;
            VkImageLayout layout;
    };

    struct Subpass {
            VkPipelineBindPoint bind_point;
            std::vector<RenderAttachmentReference> color_references;
            //std::vector<RenderAttachment> attachment; The other attachments later.
    };

    struct SubpassDependency {
            uint32_t src_subpass;
            uint32_t dst_subpass;
            VkPipelineStageFlags src_stage_mask;
            VkPipelineStageFlags dst_stage_mask;
            VkAccessFlags src_access_mask;
            VkAccessFlags dst_access_mask;
    };

    class RenderPass {
    public:
    RenderPass(Device& device, std::vector<Subpass> subpasses, std::vector<RenderAttachment> attachments, std::vector<SubpassDependency> dependencies);
    ~RenderPass();
    void create(Device& device, std::vector<Subpass> subpasses, std::vector<RenderAttachment> attachments, std::vector<SubpassDependency> dependencies);
    VkRenderPass& get() {return _render_pass;}
    private:
    VkAttachmentReference _create_reference(RenderAttachmentReference ref);
    VkRenderPass _render_pass;
    //depth coming soon
    //maybe ill remove if i decide to use dynamic rendering
    Device& _device;
    };

    class Framebuffer {
        public:
            Framebuffer();
            ~Framebuffer();
            void create();
            VkFramebuffer& get() {return _framebuffer;}
        private:
            VkFramebuffer _framebuffer;
            Device& _device;
    };

}
