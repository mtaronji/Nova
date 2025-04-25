#include "FramebufferGenerator.hpp"

FramebufferGenerator::  FramebufferGenerator(VkDevice device,
    VkRenderPass renderPass,
    std::shared_ptr<SwapchainManager> swapchainManager)
{
   
   CreateFrameBuffers(device, renderPass, swapchainManager);
}

void FramebufferGenerator::CreateFrameBuffers(VkDevice device, VkRenderPass renderPass, std::shared_ptr<SwapchainManager> swapchainManager){
    auto imageViews = swapchainManager->GetImageViews();
    auto extent = swapchainManager->GetExtent();
    this->framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); ++i) {
        VkImageView attachments[] = { imageViews[i] };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void FramebufferGenerator::Cleanup(const VkDevice& device) {
    for (auto framebuffer : this->framebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    framebuffers.clear();
}

void FramebufferGenerator::ReCreateFrameBuffers(VkDevice device, VkRenderPass renderPass, std::shared_ptr<SwapchainManager> swapchainManager){
    CreateFrameBuffers(device,renderPass, swapchainManager);
}
