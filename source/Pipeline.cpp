#include "Pipeline.hpp"
#include "GPU.hpp"
#include "Shader.hpp"


Pipeline::Pipeline(
                GPU& gpu, 
                VkRenderPass renderPass, 
                VkDescriptorSetLayout descriptorSetLayout,
                Shader<VK_SHADER_STAGE_VERTEX_BIT> vshader, 
                Shader<VK_SHADER_STAGE_FRAGMENT_BIT> fshader )
    : gpu(gpu), descripterSetLayout(descripterSetLayout),renderPass(renderPass), vshader(vshader), fshader(fshader) {

    CreateGraphicsPipeline();
}

Pipeline::~Pipeline() {
    vkDestroyPipeline(gpu.GetVkDevice(), pipeline, nullptr);
    vkDestroyPipelineLayout(gpu.GetVkDevice(), pipelineLayout, nullptr);
}


Shader<VK_SHADER_STAGE_FRAGMENT_BIT> Pipeline::GetFragShader(){
    return this->fshader;
}
Shader<VK_SHADER_STAGE_VERTEX_BIT> Pipeline::GetVertexShader(){
    return this->vshader;
}

void Pipeline::CreateGraphicsPipeline() {
    this->fshader.GetPipelineStageInfo();
    VkPipelineShaderStageCreateInfo shaderStages[] = {this->vshader.GetPipelineStageInfo(), this->fshader.GetPipelineStageInfo()};

    // Pipeline config (vertex input, assembly, viewport, rasterizer, etc.)
    // Keep simple for now
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = 800.0f; // placeholder
    viewport.height = 600.0f;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{{0, 0}, {800, 600}};

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

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
    layoutInfo.pSetLayouts = &this->descripterSetLayout;

    vkCreatePipelineLayout(gpu.GetVkDevice(), &layoutInfo, nullptr, &pipelineLayout);

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
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

