#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include "GPU.hpp"
#include "Shader.hpp"


class Pipeline {
public:
    Pipeline(
        const GPU& gpu, 
        VkRenderPass renderPass, 
        VkDescriptorSetLayout descriptorSetLayout, 
        Shader<VK_SHADER_STAGE_VERTEX_BIT> vshader, 
        Shader<VK_SHADER_STAGE_FRAGMENT_BIT> fshader
    );

    ~Pipeline();

    VkPipeline Get() const { return pipeline; }
    VkPipelineLayout GetLayout() const { return pipelineLayout; }
    Shader<VK_SHADER_STAGE_FRAGMENT_BIT> GetFragShader();
    Shader<VK_SHADER_STAGE_VERTEX_BIT> GetVertexShader();


private:
    const GPU& gpu;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<char> ReadShaderFile(const std::string &location);
    VkDescriptorSetLayout descripterSetLayout;
    VkRenderPass renderPass;
    Shader<VK_SHADER_STAGE_VERTEX_BIT> vshader;
    Shader<VK_SHADER_STAGE_FRAGMENT_BIT> fshader;

    void CreateGraphicsPipeline();
};