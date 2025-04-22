#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include<vector>
#include "GPU.hpp"



class Shader {
    public:
        Shader() = delete;
        Shader(GPU& gpu, const std::string& filepath, VkShaderStageFlagBits stage);
        ~Shader();
    
        VkShaderModule GetModule() const { return shaderModule; }
        VkShaderStageFlagBits GetStageFlag() const { return shaderStage; }
    
        VkPipelineShaderStageCreateInfo GetPipelineShaderStageCreateInfo() const;

    
    protected:
        GPU& gpu;
        VkShaderModule shaderModule;
        VkShaderStageFlagBits shaderStage;
        VkPipelineShaderStageCreateInfo pipelineShaderStageInfo{};


        std::vector<char> ReadFile(const std::string& path);
        void CreateShaderModule(const std::vector<char>& code);
        void CreateShaderStageInfo();
    };