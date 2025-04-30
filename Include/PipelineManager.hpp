#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <memory>
#include "GPU.hpp"
#include "RenderPassManager.hpp"
#include "GraphicsPipelineLoader.hpp"
#include "DescriptorAllocator.hpp"

class PipelineManager {

    public:
        PipelineManager();
        PipelineManager(
            std::shared_ptr<GPU> gpu, 
            std::shared_ptr<RenderPassManager> renderpassManager,
            std::shared_ptr<DescriptorAllocator> descriptorAllocator
        );
        
        ~PipelineManager();
        VkPipeline GetPipeline() const { return pipeline; }
        VkPipelineLayout GetPipelineLayout()const {return pipelineLayout;}
        void LoadConfig(const std::string configFile);


    protected:
        std::shared_ptr<RenderPassManager> renderpassManager;
        std::shared_ptr<DescriptorAllocator> descriptorAllocator;
        std::shared_ptr<GPU> gpu;

        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;
        
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
        std::vector<char> vertexShaderCode;
        std::vector<char> fragmentShaderCode;
        std::vector<char> computeShaderCode;
        std::vector<char> geometryShaderCode;
        std::vector<VkVertexInputBindingDescription> bdescriptions;
        std::vector<VkVertexInputAttributeDescription> adescriptions;
        VkPipelineInputAssemblyStateCreateInfo inputAssembly;
        VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineColorBlendStateCreateInfo colorBlending;
        std::vector<VkPipelineColorBlendAttachmentState> colorblendAttachments;
        std::vector<VkDynamicState> dynamicStates;
        std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorBindingsPerSet;

        
        void CreateGraphicsPipeline();
        void CreateDescriptorSetLayout();
        void AllocateDescriptorSets();
        VkShaderModule CreateShaderModule(const std::vector<char>& code);
        void CreateDescriptorSetPool();
};
