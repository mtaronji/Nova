#include "RenderPassManager.hpp"

RenderPassManager::~RenderPassManager() {
    
}

void RenderPassManager:: NotifyRenderPassOutOfDate(){

}

RenderPassManager* RenderPassManager::Create(std::shared_ptr<GPU> gpu,std::string file){
    return new RenderPassManager(gpu, file);
}

RenderPassManager::RenderPassManager(std::shared_ptr<GPU> gpu, std::string file){
    this->gpu = gpu;
    this->info = RenderPassLoader::LoadFromFile(file,
                                                attachmentDescriptions,
                                                subpassDescriptions, 
                                                subpassdependencies,
                                                colorAtachmentRefs,
                                                depthAtachmentRefs,
                                                resolveAtachmentRefs,
                                                imageUsesForAttachments,
                                                aspectFlagsForAttachments,
                                                clearValues);
    RenderPassManager::CreateRenderPass();
}

VkRenderPassCreateInfo RenderPassManager::LoadRenderpassConfig(const std::string path){
    return RenderPassLoader::LoadFromFile(path,
                                            attachmentDescriptions,
                                            subpassDescriptions, 
                                            subpassdependencies,
                                            colorAtachmentRefs, 
                                            depthAtachmentRefs,
                                            resolveAtachmentRefs,
                                            imageUsesForAttachments,
                                            aspectFlagsForAttachments,
                                            clearValues);
    }

void RenderPassManager::SetPresentColorAttachmentFormat(){
   
}
void RenderPassManager::CreateRenderPass() {

    if (vkCreateRenderPass(gpu->GetVkDevice(), &info, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}
void RenderPassManager::Cleanup(){
    vkDestroyRenderPass(gpu->GetVkDevice(), renderPass, nullptr);
}
void RenderPassManager::UpdateColorAttachmentFormats(
    VkFormat oldColorFormat,
    VkFormat newColorFormat
){

    VkRenderPassCreateInfo newinfo;
    newinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    newinfo.attachmentCount = info.attachmentCount;
    newinfo.dependencyCount = info.dependencyCount;
    newinfo.pDependencies = info.pDependencies;
    newinfo.flags = info.flags;
    newinfo.subpassCount = info.subpassCount;
    newinfo.pSubpasses = info.pSubpasses;
    std::vector<VkAttachmentDescription> newAttachments;
    for (uint32_t i = 0; i < info.attachmentCount; ++i) {
        VkAttachmentDescription attachment = info.pAttachments[i];

        // Check if this attachment is likely part of the swapchain
        bool usesOldFormat = (attachment.format == oldColorFormat);
        bool endsInPresent = (attachment.finalLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        if (usesOldFormat && endsInPresent) {
            attachment.format = newColorFormat;         
        }
        newAttachments.push_back(attachment);
    }
    newinfo.pAttachments = newAttachments.data();
    info = newinfo;
}

void  RenderPassManager::UpdateDepthAttachmentFormats(
    VkFormat oldDepthFormat,
    VkFormat newDepthFormat
){
    VkRenderPassCreateInfo newinfo;
    newinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    newinfo.attachmentCount = info.attachmentCount;
    newinfo.dependencyCount = info.dependencyCount;
    newinfo.pDependencies = info.pDependencies;
    newinfo.flags = info.flags;
    newinfo.subpassCount = info.subpassCount;
    newinfo.pSubpasses = info.pSubpasses;
    std::vector<VkAttachmentDescription> newAttachments;
    for (uint32_t i = 0; i < info.attachmentCount; ++i) {
        VkAttachmentDescription attachment = info.pAttachments[i];
    
        bool isDepthFormat = (
            attachment.format == VK_FORMAT_D32_SFLOAT ||
            attachment.format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
            attachment.format == VK_FORMAT_D24_UNORM_S8_UINT
        );
    
        bool usesOldDepthFormat = (attachment.format == oldDepthFormat);
        bool endsInDepthLayout = (
            attachment.finalLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
            attachment.finalLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL
        );
    
        if (usesOldDepthFormat && isDepthFormat && endsInDepthLayout) {
            attachment.format = newDepthFormat;
        }
        newAttachments.push_back(attachment);
    }
    newinfo.pAttachments = newAttachments.data();
    info = newinfo;
}
