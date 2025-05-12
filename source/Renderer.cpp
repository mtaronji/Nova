#include "Renderer.hpp"
#include "RenderPassManager.hpp"
#include "PipelineManager.hpp"
#include "GPU.hpp"
#include "SwapchainManager.hpp"
#include "SyncManager.hpp"
#include "CommandManager.hpp"
#include "VulkanEngine.hpp"
#include "FramebufferGenerator.hpp"
#include "BufferOps.hpp"
#include "ResourceManager.hpp"

Renderer::Renderer(   
    std::shared_ptr<Shell> shell,
    std::shared_ptr<GPU> gpu,
    std::shared_ptr<VulkanEngine> engine,
    std::shared_ptr<SwapchainManager> swapchainmanager,
    std::shared_ptr<SyncManager> syncmanager,
    std::shared_ptr<PipelineManager> pipelinemanager,
    std::shared_ptr<RenderPassManager> renderpassmanager,
    std::shared_ptr<CommandManager> commandmanager,
    std::shared_ptr<FramebufferGenerator> framebufferContainer
    ):shell(shell),gpu(gpu), engine(engine),swapchainmanager(swapchainmanager), syncmanager(syncmanager),renderpassmanager(renderpassmanager),pipelinemanager(pipelinemanager),commandmanager(commandmanager),
    framebufferContainer(framebufferContainer)
    {
        
        
    }

Renderer::~Renderer() {}



void Renderer::DrawFrame(ResourceManager* resourceManager) {
    
    FrameSync& frame = syncmanager->GetFrame(currentFrame);
    auto device = gpu->GetVkDevice();
    auto graphicsQueue = gpu->GetGraphicsQueue();
    auto presentQueue = gpu->GetPresentQueue();
    auto commandbuffer = commandmanager->GetCommandBuffer(currentFrame);

    vkWaitForFences(device, 1, &frame.inFlight, VK_TRUE, UINT64_MAX);
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(gpu->GetVkDevice(), swapchainmanager->GetSwapchain(), UINT64_MAX, frame.imageAvailable, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapchain();
        
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    //updateUniformBuffer(currentFrame);

    vkResetFences(device, 1, &frame.inFlight);

    vkResetCommandBuffer(commandbuffer, /*VkCommandBufferResetFlagBits*/ 0);
    DrawFrameCommands(commandbuffer, imageIndex, resourceManager);

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

    VkSwapchainKHR swapChains[] = {this->swapchainmanager->GetSwapchain()};
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
                                  ResourceManager* resourceManager
                                  
                                ){
  
    auto const &renderpass = renderpassmanager->GetRenderPass();
    auto const &extent = swapchainmanager->GetExtent();
    VkBuffer vbuffer = VK_NULL_HANDLE;
    VkBuffer ibuffer = VK_NULL_HANDLE;
    
    auto square = resourceManager->RetrieveMesh("square");
    vbuffer = square->vertexResource->GetBuffer();
    ibuffer = square->indiceResource->GetBuffer();


        // if(resourceMap->contains("vertices")){
        //     vbuffer = resourceMap->at("vertices")->GetBuffer();
        // }
        // if(resourceMap->contains("indices")){
        //     ibuffer = resourceMap->at("indices")->GetBuffer();
        // }
    
 

    // auto const &pipeline = pipelinemanager->GetPipeline();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderpassmanager->GetRenderPass();
    renderPassInfo.framebuffer = framebufferContainer->GetFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchainmanager->GetExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelinemanager->GetPipeline());

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
        
        if (vbuffer != VK_NULL_HANDLE) {
           
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vbuffer, offsets);
            if(ibuffer != VK_NULL_HANDLE){
                vkCmdBindIndexBuffer(commandBuffer, ibuffer, 0, VK_INDEX_TYPE_UINT32);
                uint32_t arrayCount = square->indiceResource->GetArraySize();
                vkCmdDrawIndexed(commandBuffer, arrayCount, 1, 0, 0, 0);
            }
        }
        
        
        
        // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelinelayout, 0, 1, descriptorSets[currentFrame].data(), 0, nullptr);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    

}


void Renderer::RecreateSwapchain(){
    auto device = gpu->GetVkDevice();
    int width = 0, height = 0;

    vkDeviceWaitIdle(device);
    this->framebufferContainer->Cleanup(gpu->GetVkDevice());
    swapchainmanager->Cleanup(device);
    
    swapchainmanager =  SwapchainManager::Builder()
                        .WithGPU(gpu)
                        .WithEngine(engine)
                        .WithShell(shell)
                        .Build();
                
    this->framebufferContainer->ReCreateFrameBuffers(gpu->GetVkDevice(),renderpassmanager->GetRenderPass(), swapchainmanager);
}