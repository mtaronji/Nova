#include "Renderer.hpp"

Renderer::Renderer(const GPU& gpu, SwapchainManager& swapchain, SyncManager& syncs, const Pipeline& pipeline, std::vector<VkCommandBuffer>& commandBuffers)
    :gpu(gpu), swapchain(swapchain), pipeline(pipeline), syncs(syncs),commandBuffers(commandBuffers)
    {

    }

Renderer::~Renderer() {}

void Renderer::DrawFrame() {
    FrameSync& frame = syncs.GetFrame(currentFrame);
    auto device = gpu.GetVkDevice();
    auto graphicsQueue = gpu.GetGraphicsQueue();
    auto presentQueue = gpu.GetPresentQueue();

    vkWaitForFences(device, 1, &frame.inFlight, VK_TRUE, UINT64_MAX);
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(gpu.GetVkDevice(), swapchain.GetSwapchain(), UINT64_MAX, frame.imageAvailable, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        this->NotifySwapchainOutOfDate();
        
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    //updateUniformBuffer(currentFrame);

    vkResetFences(device, 1, &frame.inFlight);

    vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    DrawFrameCommands();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {frame.imageAvailable};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = {frame.renderFinished};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, frame.inFlight) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {this->swapchain.GetSwapchain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        swapchain
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::NotifySwapchainOutOfDate(){
    this->framebufferResized = true;
}

void Renderer::DrawFrameCommands(){

}

Renderer::Builder& Renderer::Builder::CreateFrameBuffers() {
    auto swapChainImageViews = swapchain.GetImageViews();
    swapChainFramebuffers.resize(swapChainImageViews.size());
    auto renderpass = render
    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(swapChainImageViews.size());
        framebufferInfo.pAttachments = swapChainImageViews.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    return *this;
}