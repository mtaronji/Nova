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
        static PipelineManager Create(
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
        DescriptorFile* GetDescriptorFile() { return this->descriptorFile; }
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


            //CreateTestPipeline(vertShaderModule, fragShaderModule, VK_FORMAT_B8G8R8A8_SRGB);  //test
            //return;
            
            VkShaderModule computeShaderModule{};
            if(computeShaderCode.size() > 0){
                computeShaderModule = CreateShaderModule(computeShaderCode);
                VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
                computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
                computeShaderStageInfo.module = computeShaderModule;
                computeShaderStageInfo.pName = "main";
                shaderStages.push_back(computeShaderStageInfo);
            }
            VkShaderModule geometryShaderModule{};
            if(geometryShaderCode.size() > 0){
                geometryShaderModule = CreateShaderModule(geometryShaderCode);
                VkPipelineShaderStageCreateInfo geometryShaderStageInfo{};
                geometryShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                geometryShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
                geometryShaderStageInfo.module = geometryShaderModule;
                geometryShaderStageInfo.pName = "main";
                shaderStages.push_back(geometryShaderStageInfo);
            }
            
            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};         
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.flags = 0;
            auto bindingDescription = vertex::GetBindingDescription();
            auto attributeDescriptions = vertex::GetAttributeDescriptions();

         
            if constexpr (std::is_void_v<vertex>) {
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

            const auto renderpassManager = renderpassLibrary->GetRenderpassManager(renderpassKey);

            const auto attachments = renderpassManager.GetAttachmentDescriptions();


            //// --------------------------------------------------
            //// Color attachment formats
            //// --------------------------------------------------

            //std::vector<VkFormat> colorFormats;

            //for (const auto& colorRef : renderpassManager.GetColorAttachmentRefs())
            //{
            //    const auto& attachment = attachments[colorRef.attachment];
            //    colorFormats.push_back(attachment.format);
            //}


            //// --------------------------------------------------
            //// Depth / stencil format
            //// --------------------------------------------------


            //// --------------------------------------------------
            //// Dynamic rendering info
            //// --------------------------------------------------

            //renderingInfo.colorAttachmentCount =
            //    static_cast<uint32_t>(colorFormats.size());

            //renderingInfo.pColorAttachmentFormats =
            //    colorFormats.empty()
            //    ? nullptr
            //    : colorFormats.data();
            //

            //renderingInfo.depthAttachmentFormat =
            //    depthFormat;

            //renderingInfo.stencilAttachmentFormat =
            //    stencilFormat;

            //renderingInfo.pNext = nullptr;
            // 
            pipelineLayout = {};
            if (vkCreatePipelineLayout(gpu->GetVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create pipeline layout!");
            }

            

            VkPipelineRenderingCreateInfo renderingInfo{};
            renderingInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
            renderingInfo.colorAttachmentCount =
				static_cast<uint32_t>(renderpassManager.GetColorAttachmentsCount());
			renderingInfo.pColorAttachmentFormats = renderpassManager.GetColorAttachmentFormats().data();
            renderingInfo.depthAttachmentFormat = renderpassManager.GetDepthAttachmentFormats()[0];
			renderingInfo.stencilAttachmentFormat = renderpassManager.GetStencilAttachmentFormats()[0];

			colorBlending.pAttachments = colorblendAttachments.data(); // set here so we don't have to worry about the pointer becoming invalidaed when a pipeline manager is copied
            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
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
			pipelineInfo.renderPass = VK_NULL_HANDLE;
            //pipelineInfo.renderPass = renderpassLibrary->GetRenderpassManager(renderpassKey)->GetRenderPass();  changing to dynamic rendering pipeline
            //pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
            pipelineInfo.pNext = &renderingInfo;


            if (vkCreateGraphicsPipelines(gpu->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
                throw std::runtime_error("failed to create graphics pipeline!");
            }

            vkDestroyShaderModule(gpu->GetVkDevice(), vertShaderModule, nullptr);
            vkDestroyShaderModule(gpu->GetVkDevice(), fragShaderModule, nullptr);

            /*if(computeShaderCode.size()> 0){
                    vkDestroyShaderModule(gpu->GetVkDevice(), computeShaderModule, nullptr);
            }
            if(geometryShaderCode.size() > 0){
                    vkDestroyShaderModule(gpu->GetVkDevice(), geometryShaderModule, nullptr);            
            }*/

        }
        void CreateTestPipeline(
            VkShaderModule vertShader,
            VkShaderModule fragShader,
            VkFormat swapchainFormat)
        {
            //test pipeline
            VkPipelineLayoutCreateInfo layoutInfo{};
            layoutInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

            layoutInfo.setLayoutCount = 0;
            layoutInfo.pSetLayouts = nullptr;

            layoutInfo.pushConstantRangeCount = 0;
            layoutInfo.pPushConstantRanges = nullptr;

            VkPipelineLayout testPipelineLayout;

            if (vkCreatePipelineLayout(
                gpu->GetVkDevice(),
                &layoutInfo,
                nullptr,
                &testPipelineLayout) != VK_SUCCESS)
            {
                throw std::runtime_error(
                    "Failed to create test pipeline layout"
                );
            }

            VkPipelineShaderStageCreateInfo vertStage{};
            vertStage.sType =
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertStage.stage =
                VK_SHADER_STAGE_VERTEX_BIT;
            vertStage.module =
                vertShader;
            vertStage.pName =
                "main";

            VkPipelineShaderStageCreateInfo fragStage{};
            fragStage.sType =
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragStage.stage =
                VK_SHADER_STAGE_FRAGMENT_BIT;
            fragStage.module =
                fragShader;
            fragStage.pName =
                "main";

            VkPipelineShaderStageCreateInfo stages[] = {
                vertStage,
                fragStage
            };


            // ---------------------------------------------------------
            // NO vertex buffers
            // ---------------------------------------------------------

            VkPipelineVertexInputStateCreateInfo vertexInput{};
            vertexInput.sType =
                VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

            vertexInput.vertexBindingDescriptionCount = 0;
            vertexInput.vertexAttributeDescriptionCount = 0;


            // ---------------------------------------------------------
            // Triangle
            // ---------------------------------------------------------

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType =
                VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

            inputAssembly.topology =
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

            inputAssembly.primitiveRestartEnable =
                VK_FALSE;


            // ---------------------------------------------------------
            // Dynamic viewport/scissor
            // ---------------------------------------------------------

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType =
                VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

            viewportState.viewportCount = 1;
            viewportState.scissorCount = 1;


            // ---------------------------------------------------------
            // Rasterization
            // ---------------------------------------------------------

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType =
                VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;

            rasterizer.polygonMode =
                VK_POLYGON_MODE_FILL;

            rasterizer.lineWidth = 1.0f;

            rasterizer.cullMode =
                VK_CULL_MODE_NONE;

            rasterizer.frontFace =
                VK_FRONT_FACE_COUNTER_CLOCKWISE;

            rasterizer.depthBiasEnable = VK_FALSE;


            // ---------------------------------------------------------
            // No multisampling
            // ---------------------------------------------------------

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType =
                VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

            multisampling.rasterizationSamples =
                VK_SAMPLE_COUNT_1_BIT;


            // ---------------------------------------------------------
            // No depth/stencil
            // ---------------------------------------------------------

            VkPipelineDepthStencilStateCreateInfo depthStencil{};
            depthStencil.sType =
                VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

            depthStencil.depthTestEnable = VK_FALSE;
            depthStencil.depthWriteEnable = VK_FALSE;
            depthStencil.depthCompareOp =
                VK_COMPARE_OP_ALWAYS;

            depthStencil.stencilTestEnable = VK_FALSE;


            // ---------------------------------------------------------
            // Color blending
            // ---------------------------------------------------------

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;

            colorBlendAttachment.blendEnable = VK_FALSE;


            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType =
                VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

            colorBlending.logicOpEnable = VK_FALSE;

            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments =
                &colorBlendAttachment;


            // ---------------------------------------------------------
            // Dynamic states
            // ---------------------------------------------------------

            VkDynamicState dynamicStates[] = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };

            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType =
                VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

            dynamicState.dynamicStateCount = 2;
            dynamicState.pDynamicStates =
                dynamicStates;


            // ---------------------------------------------------------
            // Dynamic rendering
            // ---------------------------------------------------------

            VkPipelineRenderingCreateInfo renderingInfo{};
            renderingInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

            renderingInfo.colorAttachmentCount = 1;

            renderingInfo.pColorAttachmentFormats =
                &swapchainFormat;

            renderingInfo.depthAttachmentFormat =
                VK_FORMAT_UNDEFINED;

            renderingInfo.stencilAttachmentFormat =
                VK_FORMAT_UNDEFINED;


            // ---------------------------------------------------------
            // Pipeline
            // ---------------------------------------------------------

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType =
                VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

            pipelineInfo.pNext =
                &renderingInfo;

            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = stages;

            pipelineInfo.pVertexInputState =
                &vertexInput;

            pipelineInfo.pInputAssemblyState =
                &inputAssembly;

            pipelineInfo.pViewportState =
                &viewportState;

            pipelineInfo.pRasterizationState =
                &rasterizer;

            pipelineInfo.pMultisampleState =
                &multisampling;

            pipelineInfo.pDepthStencilState =
                &depthStencil;

            pipelineInfo.pColorBlendState =
                &colorBlending;

            pipelineInfo.pDynamicState =
                &dynamicState;

            pipelineInfo.layout =
                testPipelineLayout;

            pipelineInfo.renderPass =
                VK_NULL_HANDLE;


            // ---------------------------------------------------------
            // Create
            // ---------------------------------------------------------

            VkResult result =
                vkCreateGraphicsPipelines(
                    gpu->GetVkDevice(),
                    VK_NULL_HANDLE,
                    1,
                    &pipelineInfo,
                    nullptr,
                    &pipeline
                );

            if (result != VK_SUCCESS)
            {
                throw std::runtime_error(
                    "Failed to create test pipeline"
                );
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
