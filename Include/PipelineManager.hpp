#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include "GPU.hpp"
#include "Shader.hpp"
#include "RenderPassManager.hpp"
#include "VulkanResourceManager.hpp"

class PipelineManager {

    public:
        PipelineManager();
        PipelineManager(
            GPU& gpu, 
            RenderPassManager& renderPassManager, 
            PipelineInfo& pipelineInfo, 
            Shader<VK_SHADER_STAGE_VERTEX_BIT>& vshader, 
            Shader<VK_SHADER_STAGE_FRAGMENT_BIT>& fshader
        );
        
        ~PipelineManager();
        VkPipeline GetPipeline() const { return pipeline; }
        VkPipelineLayout GetPipelineLayout()const {return pipelineLayout;}
        PipelineInfo GetPipelineInfo() const { return pipelineInfo; }
        Shader<VK_SHADER_STAGE_FRAGMENT_BIT> GetFragShader();
        Shader<VK_SHADER_STAGE_VERTEX_BIT> GetVertexShader();

        


    protected:
        GPU& gpu;
        VkPipeline pipeline;
        PipelineInfo pipelineInfo;
        VkPipelineLayout pipelineLayout;
        std::vector<char> ReadShaderFile(const std::string &location);
        RenderPassManager& renderPassManager;
        Shader<VK_SHADER_STAGE_VERTEX_BIT>& vshader;
        Shader<VK_SHADER_STAGE_FRAGMENT_BIT>& fshader;


        void CreateGraphicsPipeline();
};

