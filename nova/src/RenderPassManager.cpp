#include "RenderPassManager.hpp"

RenderPassManager::~RenderPassManager() {
    
}


RenderPassManager RenderPassManager::Create(std::shared_ptr<GPU> gpu,std::string file){
    return RenderPassManager(gpu, file);
}

RenderPassManager::RenderPassManager(std::shared_ptr<GPU> gpu, std::string file){
    this->gpu = gpu;
    RenderPassLoader:: LoadFromFile(file,
                                    colorAttachments,
                                    depthAttachments,
                                    stencilAttachments,
                                    depthStencilAttachments,
                                    resolveAttachments,
                                    inputAttachments,
                                    attachmentDescriptions,
                                    attachmentRefs,
                                    colorAtachmentRefs,
                                    depthAtachmentRefs,
                                    resolveAtachmentRefs,
                                    depthStencilAtachmentRefs,
                                    stencilAtachmentRefs,
                                    inputAtachmentRefs,
                                    imageUsesForAttachments,
                                    aspectFlagsForAttachments,
                                    clearValues,
                                    resolveAttachmentIndices,
                                    swapchainAttachmentIndex);
    SetAttachmentFormats();
    
}

void RenderPassManager::LoadRenderpassConfig(const std::string path){
     RenderPassLoader::LoadFromFile(path, 
                                    colorAttachments,
                                    depthAttachments,
                                    stencilAttachments,
                                    depthStencilAttachments,
                                    resolveAttachments,
                                    inputAttachments,
                                    attachmentDescriptions,
                                    attachmentRefs,
                                    colorAtachmentRefs,
                                    depthAtachmentRefs,
                                    resolveAtachmentRefs,
                                    depthStencilAtachmentRefs,
                                    stencilAtachmentRefs,
                                    inputAtachmentRefs,
                                    imageUsesForAttachments,
                                    aspectFlagsForAttachments,
                                    clearValues, 
                                    resolveAttachmentIndices,
                                    swapchainAttachmentIndex);
     SetAttachmentFormats();

}

void RenderPassManager::SetPresentColorAttachmentFormat(){
   
}

void RenderPassManager::SetAttachmentFormats() {


    for (const auto& colorRef : colorAtachmentRefs)
    {
        auto attachment = attachmentDescriptions[colorRef.attachment];
        colorAttachmentFormats.push_back(attachment.format);
        colorAttachmentDescriptions.push_back(attachment);
    }
	renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentFormats.size());

    for (const auto& depthRef : depthAtachmentRefs)
    {
        const auto& attachment = attachmentDescriptions[depthRef.attachment];
        depthAttachmentFormats.push_back(attachment.format);
		depthAttachmentDescriptions.push_back(attachment);
    }


    for (const auto& stencilRef : stencilAtachmentRefs)
    {
        auto attachment = attachmentDescriptions[stencilRef.attachment];
        stencilAttachmentFormats.push_back(attachment.format);
		stencilAttachmentDescriptions.push_back(attachment);
    }

    for (const auto& depthStencilRef : depthStencilAtachmentRefs)
    {
        auto attachment = attachmentDescriptions[depthStencilRef.attachment];
        depthStencilAttachmentFormats.push_back(attachment.format);
		depthStencilAttachmentDescriptions.push_back(attachment);
    }


    for (const auto& inputRef : inputAtachmentRefs)
    {
        auto attachment = attachmentDescriptions[inputRef.attachment];
        inputAttachmentFormats.push_back(attachment.format);
		inputAttachmentDescriptions.push_back(attachment);
    }
    for (const auto& resolveRef : resolveAtachmentRefs)
    {
        auto attachment = attachmentDescriptions[resolveRef.attachment];
        resolveAttachmentFormats.push_back(attachment.format);
		resolveAttachmentDescriptions.push_back(attachment);
    }
}
void RenderPassManager::Cleanup(){
    //vkDestroyRenderPass(gpu->GetVkDevice(), renderPass, nullptr);
}
