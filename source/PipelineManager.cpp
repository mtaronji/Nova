#include "PipelineManager.hpp"


void PipelineManager::Cleanup(){
    vkDeviceWaitIdle(gpu->GetVkDevice());

    if(pipelineLayout){
        vkDestroyPipelineLayout(gpu->GetVkDevice(), pipelineLayout, nullptr);
    }
    if(pipeline){
        vkDestroyPipeline(gpu->GetVkDevice(), pipeline, nullptr);
    }

}
PipelineManager::~PipelineManager(){

}
PipelineManager::PipelineManager(
    std::shared_ptr<GPU> gpu, 
    std::shared_ptr<RenderPassManager> renderpassManager,
    std::shared_ptr<DescriptorAllocator> descriptorAllocator
    ):gpu(gpu),renderpassManager(renderpassManager),descriptorAllocator(descriptorAllocator){

    PipelineManager::LoadConfig("Pipelines/defaultPipelineConfig copy.json");

}

void PipelineManager::WithDescriptorSetLayout(){
    descriptorAllocator->CreateDescriptorSetLayout(descriptorBindingsPerSet);
}


void PipelineManager::WithDescriptorSetPool(){
    descriptorAllocator->CreateDescriptorSetPool(descriptorBindingsPerSet);
  
}

void PipelineManager::LoadConfig(const std::string configFile) {
    
    GraphicsPipelineLoader::LoadFromFile(
                                        configFile,
                                        vertexShaderCode,
                                        fragmentShaderCode,
                                        computeShaderCode,
                                        geometryShaderCode,
                                        inputAssembly,
                                        rasterizerCreateInfo,
                                        depthStencil,
                                        multisampling,
                                        colorBlending,
                                        colorblendAttachments,
                                        dynamicStates,
                                        descriptorBindingsPerSet,
                                        descriptorNames
    );
    
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


