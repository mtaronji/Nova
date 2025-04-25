#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include<vector>
#include "GPU.hpp"
#include <fstream>
#include <memory>

class Shader {
    public:
       
        Shader(std::shared_ptr<GPU> gpu, const std::string& filepath, VkShaderStageFlagBits shaderStage);
        ~Shader();
        static std::vector<char> ReadFile(const std::string& path);
        VkShaderModule GetModule() const { return shaderModule; }
        VkShaderStageFlagBits GetStageFlag() const { return shaderStage; }
    
        VkPipelineShaderStageCreateInfo GetPipelineShaderStageCreateInfo() const;

    
    protected:
        std::shared_ptr<GPU> gpu;
        VkShaderModule shaderModule;
        VkShaderStageFlagBits shaderStage;
        VkPipelineShaderStageCreateInfo pipelineShaderStageInfo{};



        void CreateShaderModule(const std::vector<char>& code);
        void CreateShaderStageInfo();
    };