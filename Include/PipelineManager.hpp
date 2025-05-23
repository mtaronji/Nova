#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <memory>
#include "GPU.hpp"
#include "GraphicsPipelineLoader.hpp"
#include "Mesh.hpp"
#include <type_traits>
#include "DescriptorAllocator.hpp"
#include "RenderPassManager.hpp"

class PipelineManager {

    public:
        PipelineManager();
        PipelineManager(
            std::shared_ptr<GPU> gpu, 
            std::shared_ptr<RenderPassManager> renderpassManager,
            std::shared_ptr<DescriptorAllocator> descriptorAllocator
        );
        
        ~PipelineManager();
        void Cleanup();
        VkPipeline GetPipeline() const { return pipeline; }
        VkPipelineLayout GetPipelineLayout()const {return pipelineLayout;}
        std::vector<std::string> GetDescriptorNames() const {return descriptorNames;}
        void LoadConfig(const std::string configFile);

  
        void WithDescriptorSetLayout();
        void WithDescriptorSetPool();

        template <typename T>  //this is the type of vertex
        void CreateGraphicsPipeline(){
            std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {};

            VkShaderModule vertShaderModule = CreateShaderModule(vertexShaderCode);
            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = vertShaderModule;
            vertShaderStageInfo.pName = "main";
            shaderStages.push_back(vertShaderStageInfo);
        
            VkShaderModule fragShaderModule = CreateShaderModule(fragmentShaderCode);
            VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = fragShaderModule;
            fragShaderStageInfo.pName = "main";
            shaderStages.push_back(fragShaderStageInfo);
            
            VkShaderModule computeShaderModule{};
            if(computeShaderCode.size() > 0){
                computeShaderModule = CreateShaderModule(computeShaderCode);
                VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
                vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                vertShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
                vertShaderStageInfo.module = computeShaderModule;
                vertShaderStageInfo.pName = "main";
                shaderStages.push_back(computeShaderStageInfo);
            }
            VkShaderModule geometryShaderModule{};
            if(geometryShaderCode.size() > 0){
                geometryShaderModule = CreateShaderModule(geometryShaderCode);
                VkPipelineShaderStageCreateInfo geometryShaderStageInfo{};
                vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                vertShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
                vertShaderStageInfo.module = geometryShaderModule;
                vertShaderStageInfo.pName = "main";
                shaderStages.push_back(geometryShaderStageInfo);
            }
            
            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};         
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.flags = 0;
            auto bindingDescription = T::GetBindingDescription();
            auto attributeDescriptions = T::GetAttributeDescriptions();

            if (std::is_void<T>::value) {
                vertexInputInfo.vertexBindingDescriptionCount = 0;
                vertexInputInfo.pVertexBindingDescriptions = nullptr;
                vertexInputInfo.vertexAttributeDescriptionCount = 0;
                vertexInputInfo.pVertexAttributeDescriptions = nullptr;
            }
            else{
                vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
                vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
                vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
                vertexInputInfo.vertexBindingDescriptionCount = 1;       
            }
        
            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;    // We will set viewports dynamically later
            viewportState.scissorCount = 1;     // We will set scissors dynamically later
            viewportState.pViewports = nullptr;
            viewportState.pScissors = nullptr;
        
            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t> (dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();
        
        
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorAllocator->GetDescriptorSetLayouts().size());
            pipelineLayoutInfo.pSetLayouts = descriptorAllocator->GetDescriptorSetLayouts().data();
            pipelineLayoutInfo.pushConstantRangeCount = 0;       // No push constants
            pipelineLayoutInfo.pPushConstantRanges = nullptr;
        
        
            pipelineLayout = {};
            if (vkCreatePipelineLayout(gpu->GetVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create pipeline layout!");
            }
        
            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages.data();
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizerCreateInfo;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = &depthStencil;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &dynamicState;
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = renderpassManager->GetRenderPass();
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        
        
            if (vkCreateGraphicsPipelines(gpu->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
                throw std::runtime_error("failed to create graphics pipeline!");
            }

            vkDestroyShaderModule(gpu->GetVkDevice(), vertShaderModule, nullptr);
            vkDestroyShaderModule(gpu->GetVkDevice(), fragShaderModule, nullptr);

            if(computeShaderCode.size()> 0){
                 vkDestroyShaderModule(gpu->GetVkDevice(), computeShaderModule, nullptr);
            }
            if(geometryShaderCode.size() > 0){
                 vkDestroyShaderModule(gpu->GetVkDevice(), geometryShaderModule, nullptr);            
            }

        }


    protected:
        std::shared_ptr<RenderPassManager> renderpassManager;
        std::shared_ptr<DescriptorAllocator> descriptorAllocator;
        std::shared_ptr<GPU> gpu;

        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
        std::vector<char> vertexShaderCode;
        std::vector<char> fragmentShaderCode;
        std::vector<char> computeShaderCode;
        std::vector<char> geometryShaderCode;
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
        VkPipelineDepthStencilStateCreateInfo depthStencil = {};
        VkPipelineMultisampleStateCreateInfo multisampling = {};
        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        std::vector<VkPipelineColorBlendAttachmentState> colorblendAttachments;
        std::vector<VkDynamicState> dynamicStates;
        std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorBindingsPerSet;
        std::vector<std::string> descriptorNames;

        
        void CreateDescriptorSetLayout();
        void AllocateDescriptorSets();
        VkShaderModule CreateShaderModule(const std::vector<char>& code);
        void CreateDescriptorSetPool();
    private:
        uint32_t MAX_FRAMES = 3;
};
