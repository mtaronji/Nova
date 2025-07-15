#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include "GPU.hpp"
#include "RenderpassLibrary.hpp"
#include "DescriptorsetLoader.hpp"

class PipelineManager {

    public:
        PipelineManager() = delete;
        static PipelineManager* Create(
            std::shared_ptr<GPU> gpu, 
            std::shared_ptr<RenderpassLibrary> renderpassLibrary,
            std::string PipelineFile
        );
        
        ~PipelineManager();
        void Cleanup();
        VkPipeline GetPipeline() const { return pipeline; }
        VkPipelineLayout GetPipelineLayout()const {return pipelineLayout;}
        std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts() {return  descriptorSetLayouts;}
        std::unordered_map<uint32_t, std::vector<std::string>>& GetDescriptorNames(){ return descriptorNames;}

        std::string GetRenderpassKey() const {return renderpassKey;}
        std::string GetVertexType() const {return vertexType;}
        std::string GetDescriptorFileName() const { return descriptorFileName; }
        void SetDescriptorFile(DescriptorFile* file) { this->descriptorFile = file; }
        std::vector<uint32_t> GetDescriptorSetIndexes() { return descriptorSetIndexes; }
        VkPushConstantRange& GetPushConstantRange(uint32_t index) {return pushConstantRanges[index]; }

        std::vector<uint32_t> GetDescriptorSetsSizes(uint32_t maxFrames){
            std::vector<uint32_t> setCopies;
            for(auto & b : this->descriptorFile->descriptorSetsInfo){
                setCopies.push_back(b.scope == DesciptorUsageScope::USAGE_SCOPE_PER_FRAME ? maxFrames : 1);
            }
            return setCopies;
        }

        void LoadConfig(const std::string configFile);

        std::vector<std::vector<VkDescriptorSetLayoutBinding>>& GetDescriptorSetBindings() {
            return descriptorFile->GetDescriptorBindings();
        }

        DescriptorSetInfo& GetDescriptorInfo() { return descriptorSetInfo; }
        void SetDescriptorInfo(DescriptorSetInfo& info) { descriptorSetInfo = info; }

        template <typename vertex>  //this is the type of vertex
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
            auto bindingDescription = vertex::GetBindingDescription();
            auto attributeDescriptions = vertex::GetAttributeDescriptions();

            if (std::is_void<vertex>::value) {
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
            pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
            pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

            if(pushConstantRanges.size() > 0){
                pipelineLayoutInfo.pushConstantRangeCount = pushConstantRanges.size();
                pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();
            }
            else{
                pipelineLayoutInfo.pushConstantRangeCount = 0;       // No push constants
                pipelineLayoutInfo.pPushConstantRanges = nullptr;
            }


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
            pipelineInfo.renderPass = renderpassLibrary->GetRenderpassManager(renderpassKey)->GetRenderPass();
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
        PipelineManager(
            std::shared_ptr<GPU> gpu, 
            std::shared_ptr<RenderpassLibrary> renderpassLibrary,
            std::string PipelineFile
        );
        std::shared_ptr<RenderpassLibrary> renderpassLibrary;
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
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        std::unordered_map<uint32_t, std::vector<std::string>> descriptorNames; //per set
        std::vector<VkPushConstantRange> pushConstantRanges;
        std::string renderpassKey;
        std::string vertexType;
        DescriptorSetInfo descriptorSetInfo;
        std::vector<uint32_t> descriptorSetIndexes;
        std::string descriptorFileName;
        DescriptorFile* descriptorFile;
        
        /*void CreateDescriptorSetLayout();
        void AllocateDescriptorSets();*/
        VkShaderModule CreateShaderModule(const std::vector<char>& code);
        
        /*void CreateDescriptorSetPool();*/
    private:
        //uint32_t MAX_FRAMES = 3;
        
};
