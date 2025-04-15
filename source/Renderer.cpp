#include "Renderer.hpp"

Renderer::Renderer(const GPU& gpu, const SwapchainManager& swapchain, VkRenderPass renderPass, VkDescriptorSetLayout layout)
    : gpu(gpu), swapchain(swapchain), renderPass(renderPass), pipeline(gpu, renderPass, layout) {}

Renderer::~Renderer() {}

void Renderer::DrawFrame() {
    // Placeholder draw call
    // - Acquire image
    // - Begin command buffer
    // - Begin render pass
    // - Bind pipeline, descriptor sets, vertex buffers
    // - Draw
    // - End render pass
    // - Submit & present
}