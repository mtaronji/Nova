#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "GPU.hpp"
#include "Shader.hpp"
#include "RenderPassManager.hpp"
#include "PipelineConfig.hpp"

class PipelineManager {

    public:
        PipelineManager();
        PipelineManager(
            GPU& gpu, 
            RenderPassManager& renderPassManager, 
            PipelineConfig& pipelineConfig, 
            std::unordered_map<std::string,Shader> shaders
        );
        
        ~PipelineManager();
        VkPipeline GetPipeline() const { return pipeline; }
        VkPipelineLayout GetPipelineLayout()const {return pipelineLayout;}
        PipelineConfig GetPipelineInfo() const { return pipelineConfig; }
        Shader GetShader(std::string shaderName) const {return shaders.at(shaderName);}


    protected:
        GPU& gpu;
        VkPipeline pipeline;
        PipelineConfig pipelineConfig;
        VkPipelineLayout pipelineLayout;
        std::vector<char> ReadShaderFile(const std::string &location);
        RenderPassManager& renderPassManager;
        std::unordered_map<std::string,Shader> shaders;


        void CreateGraphicsPipeline();
};

