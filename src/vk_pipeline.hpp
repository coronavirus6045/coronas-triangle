#pragma once

#include "common.hpp"


namespace HelloTriangle {
    class pipeline_state {
        public:
        pipeline_state(VkDevice& device_arg);
        ~pipeline_state();

        void create_graphics_pipeline(VkExtent2D swapChainExtent);
        VkShaderModule create_shader_module(const std::vector<char>& code);
        void create_render_pass(VkFormat swapChainImageFormat);

        VkPipeline& get_graphics_pipeline() {return graphicsPipeline;}
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