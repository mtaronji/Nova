#include "Renderer.hpp"


Renderer::Renderer(GPU& gpu, 
    VulkanEngine &engine,
    SwapchainManager& swapchainmanager, 
    SyncManager& syncsmanager, 
    RenderPassManager& renderpassmanager,
    const PipelineManager& pipelinemanager, 
    CommandManager& commandmanager
    ):gpu(gpu), engine(engine),swapchainmanager(swapchainmanager), syncsmanager(syncsmanager),renderpassmanager(renderpassmanager),pipelinemanager(pipelinemanager),commandmanager(commandmanager)
    {
        assert(commandmanager.GetFrameCount() > 0);
        
    }

Renderer::~Renderer() {}

void Renderer::DrawFrame() {
    FrameSync& frame = syncsmanager.GetFrame(currentFrame);
    auto device = gpu.GetVkDevice();
    auto graphicsQueue = gpu.GetGraphicsQueue();
    auto presentQueue = gpu.GetPresentQueue();
    auto commandbuffer = commandmanager.GetCommandBuffer(currentFrame);

    vkWaitForFences(device, 1, &frame.inFlight, VK_TRUE, UINT64_MAX);
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(gpu.GetVkDevice(), swapchainmanager.GetSwapchain(), UINT64_MAX, frame.imageAvailable, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapchain();
        
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    //updateUniformBuffer(currentFrame);

    vkResetFences(device, 1, &frame.inFlight);

    vkResetCommandBuffer(commandbuffer, /*VkCommandBufferResetFlagBits*/ 0);
    DrawFrameCommands(commandbuffer, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {frame.imageAvailable};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandbuffer;

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

    VkSwapchainKHR swapChains[] = {this->swapchainmanager.GetSwapchain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::NotifySwapchainOutOfDate(){
    this->framebufferResized = true;
}

void Renderer::DrawFrameCommands(VkCommandBuffer commandBuffer, 
                                  uint32_t imageIndex, 
                                  VkBuffer vbuffers[] = VK_NULL_HANDLE, 
                                  VkBuffer ibuffer = VK_NULL_HANDLE
                                  
                                ){
  
    auto const &renderpass = renderpassmanager.GetRenderPass();
    auto const &extent = swapchainmanager.GetExtent();
    auto const &pipeline = pipelinemanager.GetPipeline();
    auto const &pipelinelayout = pipelinemanager.GetPipelineLayout();
    auto const &pipelineinfo = pipelinemanager.GetPipelineInfo();
    auto const &pipelined = pipelineinfo.descriptorSetsPerBinding[] 

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderpassmanager.GetRenderPass();
    renderPassInfo.framebuffer = framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) extent.width;
        viewport.height = (float) extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        if (vbuffers != VK_NULL_HANDLE) {
           
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vbuffers, offsets);
            if(ibuffer != VK_NULL_HANDLE){
                vkCmdBindIndexBuffer(commandBuffer, ibuffer, 0, VK_INDEX_TYPE_UINT16);
            }
        }
      
        
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelinelayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);

        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

Renderer::Builder& Renderer::Builder::CreateFrameBuffers() {

    auto device = gpu->GetVkDevice();
    auto swapChainImageViews = swapchainmanager->GetImageViews();
    framebuffers = & std::vector<VkFramebuffer>(swapChainImageViews.size());
    framebuffers->resize(swapChainImageViews.size());
    auto renderpass = renderpassmanager->GetRenderPass();
    auto extent = swapchainmanager->GetExtent();
    auto framebufferRef = *framebuffers;

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderpass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(swapChainImageViews.size());
        framebufferInfo.pAttachments = swapChainImageViews.data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;


        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &(*framebuffers)[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    return *this;
}

//NOT a duplicate. One is for the builder of the renderer 
//and this one is for the actual renderer when we need a resize
void Renderer::CreateFrameBuffers(){
    auto device = gpu.GetVkDevice();
    auto swapChainImageViews = swapchainmanager.GetImageViews();
    framebuffers = std::vector<VkFramebuffer>(swapChainImageViews.size());
    framebuffers.resize(swapChainImageViews.size());
    auto renderpass = renderpassmanager.GetRenderPass();
    auto extent = swapchainmanager.GetExtent();

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderpass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(swapChainImageViews.size());
        framebufferInfo.pAttachments = swapChainImageViews.data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}
void Renderer::DestroyFrameBuffers() {
    
    for (VkFramebuffer framebuffer : framebuffers) {
        if (framebuffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(gpu.GetVkDevice(), framebuffer, nullptr);
        }
    }
    framebuffers.clear();
}

void Renderer::RecreateSwapchain(){
    auto device = gpu.GetVkDevice();
    int width = 0, height = 0;

    vkDeviceWaitIdle(device);
    DestroyFrameBuffers();
    swapchainmanager.Cleanup(device);
    
    swapchainmanager = ::SwapchainManager::Builder()
                        .SetGPU(this->gpu)
                        .SetVulkanEngine(this->engine)
                        .QuerySwapChainSupport()
                        .ChooseSwapSurfaceFormat()
                        .ChooseSwapPresentMode()
                        .ChooseSwapExtent()
                        .Build();
                
    // createDepthResources();
    CreateFrameBuffers();
}