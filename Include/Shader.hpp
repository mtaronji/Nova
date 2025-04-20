#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include<vector>
#include "GPU.hpp"


template<VkShaderStageFlagBits T>
class Shader {
    public:
        Shader(const GPU& gpu, const std::string& filepath);
        ~Shader();
    
        VkShaderModule GetModule() const { return shaderModule; }
        VkShaderStageFlagBits GetStageFlag() const { return stage; }
    
        VkPipelineShaderStageCreateInfo GetPipelineStageInfo() const;

    
    protected:
        const GPU& gpu;
        VkShaderModule shaderModule;
        VkShaderStageFlagBits stage;
        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        VkPipelineShaderStageCreateInfo stage;


        std::vector<char> ReadFile(const std::string& path);
        void CreateShaderModule(const std::vector<char>& code);
        void CreateShaderStageInfo();
    };