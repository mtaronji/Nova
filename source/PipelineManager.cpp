#include "PipelineManager.hpp"

PipelineManager::PipelineManager(
        GPU& gpu, 
        RenderPassManager& renderPassManager, 
        PipelineInfo& pipelineInfo, 
        std::unordered_map<std::string,Shader> shaders
    )
    : gpu(gpu), renderPassManager(renderPassManager), pipelineInfo(pipelineInfo),shaders(shaders) {

    CreateGraphicsPipeline();
}

PipelineManager::~PipelineManager() {
    vkDestroyPipeline(gpu.GetVkDevice(), pipeline, nullptr);
    vkDestroyPipelineLayout(gpu.GetVkDevice(), pipelineLayout, nullptr);
}




void PipelineManager::CreateGraphicsPipeline() {
    auto fshader = GetShader("frag");
    auto vshader = GetShader("vert");
    this->fshader.GetPipelineStageInfo();
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    shaderStages.push_back(this->vshader.GetPipelineStageInfo());
    shaderStages.push_back(this->fshader.GetPipelineStageInfo());

    // Pipeline config (vertex input, assembly, viewport, rasterizer, etc.)
    // Keep simple for now
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;


    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState blendAttachment{};
    blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                     VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &blendAttachment;

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = this->pipelineInfo.descriptorSetLayouts.data();

    vkCreatePipelineLayout(gpu.GetVkDevice(), &layoutInfo, nullptr, &pipelineLayout);

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &this->pipelineInfo.viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPassManager.GetRenderPass();
    pipelineInfo.subpass = 0;

    vkCreateGraphicsPipelines(gpu.GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);

}

std::vector<char> readFile(const std::string& filename) {

    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

