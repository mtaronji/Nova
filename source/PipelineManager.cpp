#include "PipelineManager.hpp"
#include "GraphicsPipelineLoader.hpp"

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
PipelineManager* PipelineManager::Create(   
                                std::shared_ptr<GPU> gpu, 
                                std::shared_ptr<RenderpassLibrary> renderpassLibrary,
                                std::string pipelineFile){

        return new PipelineManager(gpu,renderpassLibrary, pipelineFile);
}
PipelineManager::PipelineManager(
                                std::shared_ptr<GPU> gpu, 
                                std::shared_ptr<RenderpassLibrary> renderpassLibrary,
                                std::string pipelineFile
                                ):gpu(gpu),renderpassLibrary(renderpassLibrary){

    PipelineManager::LoadConfig(pipelineFile);

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
                                        descriptorNames,
                                        pushConstantRanges,
                                        renderpassKey,
                                        vertexType
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



