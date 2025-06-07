#pragma once

#include "common.hpp"
#include "vk_shader.hpp"
#include "vk_descriptor.hpp"
#include <vulkan/vulkan_core.h>


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
            PipelineLayout();
            void set_layouts(std::vector<DescriptorLayout> layouts);
            void create(VkPipelineLayoutCreateFlags flags);
            void get();
        private:
            VkPipelineLayout _pipeline_layout;
            std::vector<VkDescriptorSetLayout> _layouts;
    };

    //do we NEED this? ill maybe separate to PipelineInfo and its
    class PipelineMaker {
    public:
    PipelineMaker();
    ~PipelineMaker();
    //Resets
    //void init();

    //dynamic states BABY
    void set_viewport_count(uint32_t count);
    void set_scissor_count(uint32_t count);
    //this is too much, another rewrite but its easier
    void set_input_topology(VkPrimitiveTopology topology, bool primitive_restart);

    void set_vertex_attribute_description(VkVertexInputAttributeDescription description);
    void set_vertex_binding_description(VkVertexInputBindingDescription description);

    void set_color_blend_logic_op();
    void set_color_blend_attachment();

    void set_multisampling(bool enable, VkSampleCountFlagBits sample_count);

    void set_descriptor_layout();

    void set_rasterizer_polygon_mode(VkPolygonMode mode, float line_width);
    void set_rasterizer_cull_mode(VkCullModeFlags flags, VkFrontFace front_face);
    void set_shader();

    PipelineHandle create_graphics_pipeline();

    void delete_pipeline();
    private:
    std::vector<VkPipelineShaderStageCreateInfo> _shader_stage_infos;
    std::vector<VkVertexInputBindingDescription> binding_descriptions;
    std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
    std::vector<HelloTriangle::DescriptorLayout> descriptor_layouts;
    VkPipelineRasterizationStateCreateInfo rasterizer_info;
    VkPipelineMultisampleStateCreateInfo multisampling_info;
    VkPipelineColorBlendStateCreateInfo color_blending_info;
    VkPipelineVertexInputStateCreateInfo vertex_input_info;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
    VkPipelineLayoutCreateInfo pipeline_layout_info;
    VkPipelineLayout p_pipeline_layout;
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

    struct Subpass {
            VkPipelineBindPoint bind_point;
            std::vector<RenderAttachment> color_attachments;
            //std::vector<RenderAttachment> attachment;
    };

    class RenderPass {
    public:
    RenderPass();
    ~RenderPass();
    void create();
    void set_subpass();
    void set_attachment();
    VkRenderPass& get();
    private:
    VkRenderPass _render_pass;
    std::vector<VkAttachmentDescription> _attachments;
    //depth coming soon
    //maybe ill remove if i decide to use dynamic rendering
    std::vector<VkSubpassDescription> _subpasses;
    std::vector<VkSubpassDependency> _dependencies;
    }
    //since this thing as TOO many things to fill out, i'll do it in a struct (reinventing the same thing)




    class pipeline_state {
        public:
        pipeline_state(VkDevice& device_arg);
        ~pipeline_state();

        void create_graphics_pipeline(VkExtent2D swapChainExtent, VkDescriptorSetLayout descriptorSetLayout);
        VkShaderModule create_shader_module(const std::vector<char>& code);
        void create_render_pass(VkFormat swapChainImageFormat);

        VkPipeline& get_graphics_pipeline() {return graphicsPipeline;}
        VkPipelineLayout& get_pipeline_layout() {return pipelineLayout;}
        VkRenderPass& get_render_pass() {return renderPass;}

        private:
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
        VkRenderPass renderPass;

        VkDevice& device;
        //Query functions need to be STATIC.
    };
}

/*
guide for myself, organizing classes like this
SEPARATE BY NEWLINE.
public:
constructor
destructor
main funcs
getters
static funcs (e.g. query)
private
main objects
objects (references, initialized by constructor)

*/
