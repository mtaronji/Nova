#include "RenderPass.hpp"
#include "GPU.hpp"

RenderPass::RenderPass(const GPU& gpu, VkFormat colorFormat) : gpu(gpu) {
    createRenderPass(colorFormat);
}

RenderPass::~RenderPass() {
    vkDestroyRenderPass(gpu.GetVkDevice(), renderPass, nullptr);
}

void RenderPass::createRenderPass(VkFormat colorFormat) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = colorFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    VkRenderPassCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &colorAttachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;

    if (vkCreateRenderPass(gpu.GetVkDevice(), &info, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass.");
    }
}