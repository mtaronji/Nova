#include "PipelineManager.hpp"

PipelineManager::PipelineManager(
    std::shared_ptr<GPU> gpu, 
    std::shared_ptr<RenderPassManager> renderpassManager,
    std::shared_ptr<DescriptorAllocator> descriptorAllocator
    ):gpu(gpu),renderpassManager(renderpassManager),descriptorAllocator(descriptorAllocator){

    PipelineManager::LoadConfig("Pipelines/defaultPipelineConfig.json");
    PipelineManager::CreateDescriptorSetLayout();
    PipelineManager::CreateDescriptorSetPool();
    PipelineManager::CreateGraphicsPipeline();

}

PipelineManager::~PipelineManager() {
    vkDestroyPipeline(gpu->GetVkDevice(), pipeline, nullptr);
    vkDestroyPipelineLayout(gpu->GetVkDevice(), pipelineLayout, nullptr);
}

void PipelineManager::CreateDescriptorSetLayout(){
       
}


void PipelineManager::CreateDescriptorSetPool(){
    descriptorAllocator->CreateDescriptorSetPool(3,descriptorBindingsPerSet);
}

void PipelineManager::LoadConfig(const std::string configFile) {
    
    GraphicsPipelineLoader::LoadFromFile(
                                        configFile,
                                        vertexShaderCode,
                                        fragmentShaderCode,
                                        computeShaderCode,
                                        geometryShaderCode,
                                        bdescriptions,
                                        adescriptions,
                                        inputAssembly,
                                        rasterizerCreateInfo,
                                        multisampling,
                                        colorBlending,
                                        colorblendAttachments,
                                        dynamicStates,
                                        descriptorBindingsPerSet
    );
    
}

void PipelineManager::CreateGraphicsPipeline(){

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

    if(computeShaderCode.size() > 0){
        VkShaderModule computeShaderModule = CreateShaderModule(computeShaderCode);
        VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        vertShaderStageInfo.module = computeShaderModule;
        vertShaderStageInfo.pName = "main";
        shaderStages.push_back(computeShaderStageInfo);
    }
    if(geometryShaderCode.size() > 0){
        VkShaderModule geometryShaderModule = CreateShaderModule(geometryShaderCode);
        VkPipelineShaderStageCreateInfo geometryShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        vertShaderStageInfo.module = geometryShaderModule;
        vertShaderStageInfo.pName = "main";
        shaderStages.push_back(geometryShaderStageInfo);
    }


    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t> (bdescriptions.size());
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(adescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = bdescriptions.data();
    vertexInputInfo.pVertexAttributeDescriptions = adescriptions.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;    // We will set viewports dynamically later
    viewportState.scissorCount = 1;     // We will set scissors dynamically later
    viewportState.pViewports = nullptr;
    viewportState.pScissors = nullptr;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;
    depthStencil.stencilTestEnable = VK_FALSE;

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

    VkPipelineLayout pipelineLayout;
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
}



VkShaderModule PipelineManager::CreateShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(gpu->GetVkDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}


