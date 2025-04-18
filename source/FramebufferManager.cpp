#include "FramebufferManager.hpp"

FramebufferManager::  FramebufferManager(const VkDevice &device,
    const VkRenderPass &renderPass,
    const std::vector<VkImageView>& imageViews,
    const VkExtent2D &extent): device(device), renderPass(renderPass), imageViews(imageViews), extent(extent)
{
    Cleanup(device); // Recreate-safe

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

const std::vector<VkFramebuffer>& FramebufferManager::GetFramebuffers() const {
    return this->framebuffers;
}

void FramebufferManager::Cleanup(const VkDevice& device) {
    for (auto framebuffer : this->framebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    framebuffers.clear();
}
