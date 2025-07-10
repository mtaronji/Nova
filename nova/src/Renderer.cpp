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
#include "DescriptorAllocator.hpp"
#include "PipelineLibrary.hpp"
#include "RenderpassLibrary.hpp"
#include "FramebufferLibrary.hpp"
#include "PushConstants.hpp"
#include "Mesh.hpp"

void Renderer::Cleanup(){
   
}

Renderer::Renderer(   
   std::shared_ptr<Shell> shell,
            std::shared_ptr<GPU> gpu,
            std::shared_ptr<VulkanEngine> engine,
            std::shared_ptr<SwapchainManager> swapchainmanager,
            std::shared_ptr<SyncManager> syncmanager,
            std::shared_ptr<PipelineLibrary> pipelineLibrary,
            std::shared_ptr<RenderpassLibrary> renderpassLibrary,
            std::shared_ptr<CommandManager> commandmanager,
            std::shared_ptr<FramebufferLibrary> framebuffersLibrary,
            std::shared_ptr<DescriptorAllocator> descriptorAllocator,
            std::shared_ptr<ResourceManager> resourceManager
    ):shell(shell), gpu(gpu), engine(engine), swapchainmanager(swapchainmanager), syncmanager(syncmanager), 
    renderpassLibrary(renderpassLibrary), pipelineLibrary(pipelineLibrary), commandmanager(commandmanager),
    framebuffersLibrary(framebuffersLibrary),resourceManager(resourceManager), descriptorAllocator(descriptorAllocator)
    {
        auto extent = swapchainmanager->GetExtent();
        frameinfo.iTime = 0.0f;
        frameinfo.iResolution = {extent.width, extent.width};   

        this->MAX_FRAMES = swapchainmanager->GetImageCount();
        imagesInFlight.resize(MAX_FRAMES, VK_NULL_HANDLE);
        
        
}

void Renderer::SetFrameTime(float deltaTime){
    frameinfo.iTime += deltaTime;       
}

std::vector<std::vector<VkDescriptorSet>>& Renderer::GetPipelineDescriptorSets(std::string pipelineKey){
    return pipelineDescriptorSets[pipelineKey];
} 

VkDescriptorSet& Renderer::GetPipelineDescriptorSet(std::string pipelineKey, uint32_t frame, uint32_t set){
    return pipelineDescriptorSets[pipelineKey][frame][set];
}

std::vector<VkDescriptorSet>& Renderer::GetPipelineDescriptorSetsFrame(std::string pipelineKey, uint32_t frame){
    return pipelineDescriptorSets[pipelineKey][frame];
}
Renderer::~Renderer() {

}

void Renderer::BindPipeline(std::string pipelineKey){
    this->pipelineManager = this->pipelineLibrary->GetPipeline(pipelineKey);
    auto renderpassKey = this->pipelineManager->GetRenderpassKey();
    this->renderpassmanager = this->renderpassLibrary->GetRenderpassManager(renderpassKey);
    //renderpass and framebuffers share the same key
    this->framebufferContainer = this->framebuffersLibrary->GetFramebufferGenerator(renderpassKey);
    currentPipelineKey = pipelineKey;

}

//are allocating based on the buffer resources
//if buffer resources has copies we will create descriptorSets for each copy
//can only allocate 1 copy for max frames copies

//in general, when you allocate descriptor sets you are setting up how many slots or bindings you have. We are setting up handles for the actual buffer resources here
//when you update the descriptorsets, you are setting the actual references to the data of the descriptor set
void Renderer::AllocateAndUpdateDescriptorSets(std::string pipelineKey){
    
    auto pipelineManager = pipelineLibrary->GetPipeline(pipelineKey);
    auto& layouts = pipelineManager->GetDescriptorSetLayouts();
    auto& pipelineBindings = pipelineManager->GetDescriptorSetBindings();
    assert(pipelineBindings.size() == layouts.size());

    //create the sizes
    //we want our descriptor sets to have frame set layout
    //all descriptor sets that don't need copies will just copy the one descriptor into all sets
    pipelineDescriptorSets[pipelineKey].resize(MAX_FRAMES);  //[frame][set]
    for(auto& v : pipelineDescriptorSets[pipelineKey]){
        v.resize(layouts.size());
    }
    auto& descriptorSetResources = resourceManager->GetDescriptorSets(pipelineKey);      //[set][binding]

    assert(descriptorSetResources.size() == layouts.size());

    //check all the resouces and make sure all descriptor sets have the same number of copies
    //if we specify resources with different copies, this is an error
    //all descriptors from a set should request the same number of resources
    //set 0 might have 4 copies
    //set 1 might have just 1 copy for all it's resources, etc
    std::vector<uint32_t> descriptorSetCopies = {};
    for(uint32_t set = 0; set < descriptorSetResources.size(); set++){
        auto& resources = descriptorSetResources[set];
        auto copies = resources[0].GetCopyCount();  //each set should have the same amount of copies as they are set for the whole set. 
        descriptorSetCopies.push_back(copies);
        for(uint32_t binding = 0; binding < resources.size(); binding++){
            assert(resources[binding].GetCopyCount() == copies);
        }
    }
    for(uint32_t i = 0; i < layouts.size(); i++){
        auto copies = descriptorSetCopies[i];
        //if the descriptor set is only requesting 1 copy, we will copy what we allocate multiple times. Each frame requests the same descriptor set
        //please note anything that might change per frame should not be allocated this way
        //we are updating the descriptor sets with the buffer info as soon as we allocate
        if(copies == 1){
            VkDescriptorSet set;
            descriptorAllocator->AllocateDescriptorSet(set, layouts[i]); 
            resourceManager->UpdateDescriptorSet(pipelineKey, set, layouts[i], pipelineBindings[i], i, 1);
            for(uint32_t frame = 0; frame < MAX_FRAMES; frame++){          
                pipelineDescriptorSets[pipelineKey][frame][i] = set;
            }   
        }
        //if the descriptor is requesting per frame, we will allocate a descriptor set for each frame. 
        else if(copies == MAX_FRAMES){
            auto descriptorSets = std::vector<VkDescriptorSet>(MAX_FRAMES);
            for(uint32_t frame = 0; frame < MAX_FRAMES; frame++){          
                descriptorAllocator->AllocateDescriptorSet(descriptorSets[frame], layouts[i]); 
                resourceManager->UpdateDescriptorSet(pipelineKey, descriptorSets[frame], layouts[i], pipelineBindings[i], i, frame);
                pipelineDescriptorSets[pipelineKey][frame][i] = descriptorSets[frame];       
            }   
        }
        else{
            throw std::runtime_error("descriptors can be allocated per frame or must be singular (just 1).");
        }
    } 
}

void Renderer::AllocateDescriptorSets(std::string pipelineKey) {
    
}

void Renderer::DrawFrame() {
    FrameSync& frame = syncmanager->GetFrame(currentFrame);
    auto device = gpu->GetVkDevice();
    auto graphicsQueue = gpu->GetGraphicsQueue();
    auto presentQueue = gpu->GetPresentQueue();
    auto commandbuffer = commandmanager->GetCommandBuffer(currentFrame);

    // Wait for the fence of the current frame to ensure the GPU has finished processing it.
    vkWaitForFences(device, 1, &frame.inFlight, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    // Acquire the next available swapchain image, signal frame.imageAvailable semaphore when ready
    VkResult result = vkAcquireNextImageKHR(
        device,
        swapchainmanager->GetSwapchain(),
        UINT64_MAX,
        frame.imageAvailable,
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapchain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // If another frame is using this image, wait for its fence to ensure it finished
    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    // Reset fence for this frame to unsignaled state, ready for the GPU to signal it later
    vkResetFences(device, 1, &frame.inFlight);

    // Reset command buffer before recording commands for this frame
    vkResetCommandBuffer(commandbuffer, 0);
    DrawFrameCommands(commandbuffer, imageIndex);

    // Submit info
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { frame.imageAvailable };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandbuffer;

    VkSemaphore signalSemaphores[] = { frame.renderFinished };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, frame.inFlight) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // Present the rendered image
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapchainmanager->GetSwapchain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        RecreateSwapchain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES;
}


void Renderer::NotifySwapchainOutOfDate(){
    this->framebufferResized = true;
}

void Renderer::DrawFrameCommands(VkCommandBuffer commandBuffer, 
                                  uint32_t imageIndex
                                ){
  
    auto const &renderpass = renderpassmanager->GetRenderPass();
    auto const &extent = swapchainmanager->GetExtent();
    
    frameinfo.iframe = imageIndex;
    
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

    renderPassInfo.clearValueCount = static_cast<uint32_t>(renderpassmanager->GetClearValues().size());
    renderPassInfo.pClearValues = renderpassmanager->GetClearValues().data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineManager->GetPipeline());

    // Push constants
    const auto & range = pipelineManager->GetPushConstantRange(0);
    vkCmdPushConstants(
        commandBuffer,
        pipelineManager->GetPipelineLayout(),
        range.stageFlags,
        0,
        sizeof(frameinfo),
        &frameinfo
    );
        
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
    
    auto& square = resourceManager->GetMesh("square");
    auto& vbuffer = square.vertexResource.GetBuffer();
    auto& ibuffer = square.indiceResource.GetBuffer();
        
    if (vbuffer != VK_NULL_HANDLE) {
           
        VkDeviceSize offsets[] = {square.vertexResource.GetOffSet() + square.vertexResource.GetAlignedDataSize(256) * currentFrame};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vbuffer, offsets);
        vkCmdBindDescriptorSets(commandBuffer, 
                                VK_PIPELINE_BIND_POINT_GRAPHICS, 
                                pipelineManager->GetPipelineLayout(), 
                                0, 
                                static_cast<uint32_t>(pipelineDescriptorSets[currentPipelineKey][currentFrame].size()), 
                                pipelineDescriptorSets[currentPipelineKey][currentFrame].data(),
                                0, 
                                nullptr);

        if(ibuffer != VK_NULL_HANDLE){
            vkCmdBindIndexBuffer(commandBuffer, ibuffer, 0, VK_INDEX_TYPE_UINT16);
            uint32_t arrayCount = square.indiceResource.GetArraySize();
            vkCmdDrawIndexed(commandBuffer, arrayCount, 1, 0, 0, 0);
        }
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }  
}


void Renderer::RecreateSwapchain(){
    auto device = gpu->GetVkDevice();
    int width = 0, height = 0;

    vkDeviceWaitIdle(device);
    this->framebufferContainer->Cleanup();
    swapchainmanager->Cleanup(device);
    
    swapchainmanager->RechooseSwapExtent();

    this->swapchainmanager->CreateSwapchain();
    this->framebufferContainer->ReCreateFramebuffers();

    auto extent = swapchainmanager->GetExtent();
    frameinfo.iResolution = {extent.width, extent.height};
}

void Renderer::DestroyFrameBuffers() {

}
void Renderer::CreateFrameBuffers() {

}