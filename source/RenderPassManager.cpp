#include "RenderPassManager.hpp"
#include "GPU.hpp"
#include <cassert>

RenderPassManager::RenderPassManager(std::shared_ptr<GPU> gpu, const VkSubpassDescription& subpassDescription, const std::vector<VkAttachmentDescription>& attachmentDescriptions)
        : gpu(gpu), subpassDescription(subpassDescription),attachmentDescriptions(attachmentDescriptions){
    

    CreateRenderPass();
}

RenderPassManager::~RenderPassManager() {
    vkDestroyRenderPass(gpu->GetVkDevice(), renderPass, nullptr);
}

void RenderPassManager:: NotifyRenderPassOutOfDate(){

}


RenderPassManager::Builder& RenderPassManager::Builder::WithAttachments(const std::vector<AttachmentInfo>& attachmentInfos, std::vector<uint32_t>& preserveAttachments){
    
    this->attachmentrefs = std::vector<VkAttachmentReference>();
    this->colorAttachments = std::vector<VkAttachmentReference>();
    this->preserveAttachments = preserveAttachments;

    uint32_t index = 0;
    for(const auto& description : attachmentInfos){

        //The description I load in the index of the scription vector must also be specified for it's corresponding reference (so vulkan can find it)
        attachmentDescriptions.push_back(description.description);

        switch (description.type){
            case AttachmentInfo::Type::Color:{
                VkAttachmentReference colorAttachmentRef = {};
                colorAttachmentRef.attachment = index; 
                colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachmentrefs.push_back(colorAttachmentRef);
                colorAttachments.push_back(colorAttachmentRef);
                index++;
              
                break;
            }
            case AttachmentInfo::Type::Depth:{
                VkAttachmentReference depthStencilAttachmentRef = {};
                depthStencilAttachmentRef.attachment = index; 
                depthStencilAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                attachmentrefs.push_back(depthStencilAttachmentRef);   
                depthStencilAttachment = depthStencilAttachmentRef;               
                index++;

                break;
            }
            case AttachmentInfo::Type::Input:{
                VkAttachmentReference inputAttachmentRef = {};
                inputAttachmentRef.attachment = index; 
                inputAttachmentRef.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                attachmentrefs.push_back(inputAttachmentRef);
                inputAttachments.push_back(inputAttachmentRef);
                index++;

                break;
            }
            case AttachmentInfo::Type::Resolve:{
                VkAttachmentReference resolveAttachmentRef = {};
                resolveAttachmentRef.attachment = index; 
                resolveAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachmentrefs.push_back(resolveAttachmentRef);
                index++;

                break;
            }
        }
    }
    return *this;
}

std::shared_ptr<RenderPassManager> RenderPassManager::Builder::Build(){
    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    

    if (!colorAttachments.empty()) {
        subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
        subpassDescription.pColorAttachments = colorAttachments.data();
    }

    if (!inputAttachments.empty()) {
        subpassDescription.inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size());
        subpassDescription.pInputAttachments = inputAttachments.data();
    }

    if (!resolveAttachments.empty()) {
        subpassDescription.pResolveAttachments = resolveAttachments.data();
    }

    if (depthStencilAttachment.has_value()) {
        subpassDescription.pDepthStencilAttachment = &depthStencilAttachment.value();
    }

    if (!preserveAttachments.empty()) {
        subpassDescription.preserveAttachmentCount = static_cast<uint32_t>(preserveAttachments.size());
        subpassDescription.pPreserveAttachments = preserveAttachments.data();
    }

    return std::make_shared<RenderPassManager>(gpu, subpassDescription,attachmentDescriptions);
}
void RenderPassManager::CreateRenderPass() {


    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = attachmentDescriptions.size();
    renderPassInfo.pAttachments = attachmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    

    if (vkCreateRenderPass(gpu->GetVkDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

}