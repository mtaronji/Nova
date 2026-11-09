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
#include <chrono>

#define NOW std::chrono::high_resolution_clock::now()
#define SECONDS std::chrono::duration<float>
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
    //this->pipelineManager = this->pipelineLibrary->GetPipeline(pipelineKey);
    //auto renderpassKey = this->pipelineManager->GetRenderpassKey();
    //this->renderpassmanager = this->renderpassLibrary->GetRenderpassManager(renderpassKey);
    //renderpass and framebuffers share the same key
    //this->framebufferContainer = this->framebuffersLibrary->GetFramebufferGenerator(renderpassKey);
    currentPipelineKey = pipelineKey;

}

//are allocating based on the buffer resources
//if buffer resources has copies we will create descriptorSets for each copy
//can only allocate 1 copy for max frames copies

//in general, when you allocate descriptor sets you are setting up how many slots or bindings you have. We are setting up handles for the actual buffer resources here
//when you update the descriptorsets, you are setting the actual references to the data of the descriptor set
void Renderer::AllocateAndUpdateDescriptorSets(std::string pipelineKey){
    
    auto pipelineManager = pipelineLibrary->GetPipeline(pipelineKey);
    auto descriptorSetCopies = pipelineManager.GetDescriptorSetsSizes(MAX_FRAMES); //1 indexed
    auto& layouts = pipelineManager.GetDescriptorSetLayouts();
    auto& pipelineBindings = pipelineManager.GetDescriptorSetBindings();
    auto descriptorFile = pipelineManager.GetDescriptorFile();
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
    assert(layouts.size() == descriptorSetCopies.size());
    


    for (uint32_t i = 0; i < layouts.size(); i++) {
        if (descriptorFile->descriptorSetsInfo[i].scope == DesciptorUsageScope::USAGE_SCOPE_PER_FRAME) {
            for (uint32_t frame = 0; frame < MAX_FRAMES; frame++) {
                VkDescriptorSet set;
                descriptorAllocator->AllocateDescriptorSet(set, layouts[i]);
                resourceManager->UpdateDescriptorSet(pipelineKey, set, layouts[i], pipelineBindings[i], i, frame);
                pipelineDescriptorSets[pipelineKey][frame][i] = set;
            }
        }
        else if(descriptorFile->descriptorSetsInfo[i].scope == DesciptorUsageScope::USAGE_SCOPE_SINGULAR) {
            VkDescriptorSet set;
            descriptorAllocator->AllocateDescriptorSet(set, layouts[i]);
            resourceManager->UpdateDescriptorSet(pipelineKey, set, layouts[i], pipelineBindings[i], i, 0);
  
            for (uint32_t frame = 0; frame < MAX_FRAMES; frame++) {
                pipelineDescriptorSets[pipelineKey][frame][i] = set;
            }
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

    //// Reset fence for this frame to unsignaled state, ready for the GPU to signal it later
    vkResetFences(device, 1, &frame.inFlight);

    // Reset command buffer before recording commands for this frame

    vkResetCommandBuffer(commandbuffer, 0);


    DrawFrameCommands(commandbuffer, imageIndex);

    // Submit info
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { frame.imageAvailable };

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
    };

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandbuffer;

    VkSemaphore signalSemaphores[] = { frame.renderFinished };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    
    result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, frame.inFlight);

    if (result != VK_SUCCESS) {
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

void Renderer::TransitionImage(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageLayout& currentLayout,
    VkImageLayout newLayout,
    VkImageAspectFlags aspectMask)
{
    // Nothing to do if the layouts are already identical.
    if (currentLayout == newLayout)
        return;

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

    barrier.oldLayout = currentLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;

    barrier.subresourceRange.aspectMask = aspectMask;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    VkPipelineStageFlags srcStage =
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    VkPipelineStageFlags dstStage =
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    // ============================================================
    // SOURCE: Previous image usage
    // ============================================================

    switch (currentLayout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED:
    {
        barrier.srcAccessMask = 0;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        break;
    }

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
    {
        barrier.srcAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        srcStage =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        break;
    }

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    {
        barrier.srcAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        srcStage =
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

        break;
    }

    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
    {
        barrier.srcAccessMask = 0;

        // Presentation engine synchronization is handled
        // separately through swapchain semaphores.
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        break;
    }

    default:
    {
        assert(false && "TransitionImage: Unsupported source layout!");
        return;
    }
    }

    // ============================================================
    // DESTINATION: Next image usage
    // ============================================================

    switch (newLayout)
    {
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
    {
        barrier.dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        dstStage =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        break;
    }

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    {
        barrier.dstAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        dstStage =
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

        break;
    }

    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
    {
        barrier.dstAccessMask = 0;

        dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        break;
    }

    default:
    {
        assert(false && "TransitionImage: Unsupported destination layout!");
        return;
    }
    }

    // ============================================================
    // EXECUTE IMAGE TRANSITION
    // ============================================================

    vkCmdPipelineBarrier(
        commandBuffer,
        srcStage,
        dstStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    // Update tracked layout after recording the barrier.
    currentLayout = newLayout;
}
void Renderer::DrawFrameCommands(VkCommandBuffer commandBuffer, 
                                  uint32_t imageIndex
                                ){
  
    //auto const &renderpass = renderpassmanager->GetRenderPass();
    auto const &extent = swapchainmanager->GetExtent();
    VkResult result;
    frameinfo.iframe = imageIndex;
    
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    std::cout
        << "Command buffer: "
        << static_cast<const void*>(commandBuffer)
        << std::endl;

    assert(commandBuffer != VK_NULL_HANDLE);
    //define the image views used in this rendering operation 
    
    auto pipelineManager =
        this->pipelineLibrary->GetPipeline("msaa4.json");

  


    // ------------------------------------------------------------
// Retrieve rendering resources
// ------------------------------------------------------------

    auto& framebuffers =
        this->framebuffersLibrary->GetFramebufferGenerator("msaa4.json");

    auto& renderManager =
        this->renderpassLibrary->GetRenderpassManager("msaa4.json");

    VkRenderingInfo renderingInfo = {};

    auto& swapchainImageView =
        swapchainmanager->GetImageViews()[imageIndex];

    auto& swapchainImage =
        swapchainmanager->GetImages()[imageIndex];

    const auto& images =
        framebuffers.GetImagesForFrame(imageIndex);

    const auto& imageViews =
        framebuffers.GetImageViewsForFrame(imageIndex);

    auto& layouts =
        framebuffers.GetCurrentLayoutsForFrame(imageIndex);

    const uint32_t swapchainIndex =
        renderManager.GetSwapchainAttachmentIndex();

    const auto& attachments = renderManager.GetAttachmentDescriptions();

    // ------------------------------------------------------------
    // Retrieve attachment references
    // ------------------------------------------------------------

    const auto& colorRefs =
        renderManager.GetColorAttachmentRefs();

    const auto& resolveRefs =
        renderManager.GetResolveAttachmentRefs();

    const auto& depthRefs =
        renderManager.GetDepthAttachmentRefs();

    const auto& stencilRefs =
        renderManager.GetStencilAttachmentRefs();

    const auto& depthStencilRefs =
        renderManager.GetDepthStencilAttachmentRefs();


    // ------------------------------------------------------------
    // Make local copies of rendering attachment structures.
    //
    // These vectors remain alive until vkCmdBeginRendering has
    // consumed their pointers during command recording.
    // ------------------------------------------------------------

    auto a1 = renderManager.GetColorRenderingAttachments();
    auto a2 = renderManager.GetDepthRenderingAttachments();
    auto a3 = renderManager.GetStencilRenderingAttachments();
    auto a4 = renderManager.GetDepthStencilRenderingAttachments();


    // ------------------------------------------------------------
    // Validate attachment counts
    // ------------------------------------------------------------

    assert(a1.size() == colorRefs.size());
    assert(resolveRefs.size() <= colorRefs.size());

    assert(a2.size() == depthRefs.size());
    assert(a3.size() == stencilRefs.size());
    assert(a4.size() == depthStencilRefs.size());


    // ------------------------------------------------------------
    // Retrieve actual image handles
    // ------------------------------------------------------------

    auto getImage = [&](uint32_t index) -> VkImage
        {
            assert(index < layouts.size());

            if (index == swapchainIndex)
                return swapchainImage;

            assert(index < images.size());

            return images[index];
        };


    // ------------------------------------------------------------
    // Retrieve actual image views
    // ------------------------------------------------------------

    auto getImageView = [&](uint32_t index) -> VkImageView
        {
            if (index == swapchainIndex)
                return swapchainImageView;

            assert(index < imageViews.size());

            return imageViews[index];
        };


    // ------------------------------------------------------------
    // Transition attachment helper
    // ------------------------------------------------------------

    auto transitionAttachment =
        [&](uint32_t index,
            VkImageLayout targetLayout,
            VkImageAspectFlags aspect)
        {
            VkImage image = getImage(index);

            assert(image != VK_NULL_HANDLE);

            auto& currentLayout = layouts[index];

            if (currentLayout != targetLayout)
            {
                TransitionImage(
                    commandBuffer,
                    image,
                    currentLayout,
                    targetLayout,
                    aspect
                );
            }
        };


    // ------------------------------------------------------------
    // COLOR + RESOLVE ATTACHMENTS
    // ------------------------------------------------------------

    for (size_t i = 0; i < colorRefs.size(); ++i)
    {
        const auto& colorRef = colorRefs[i];

        auto& colorAttachment = a1[i];

        // Assign MSAA color image view
        colorAttachment.imageView =
            getImageView(colorRef.attachment);

        colorAttachment.imageLayout =
            colorRef.layout;

        // Transition MSAA color image
        transitionAttachment(
            colorRef.attachment,
            colorRef.layout,
            VK_IMAGE_ASPECT_COLOR_BIT
        );


        // Resolve attachment, if configured
        if (i < resolveRefs.size())
        {
            const auto& resolveRef = resolveRefs[i];

            assert(colorAttachment.resolveMode !=
                VK_RESOLVE_MODE_NONE);

            // Assign resolve image view
            colorAttachment.resolveImageView =
                getImageView(resolveRef.attachment);

            colorAttachment.resolveImageLayout =
                resolveRef.layout;

            // Ensure source and destination are different images
            assert(
                getImage(colorRef.attachment) !=
                getImage(resolveRef.attachment)
            );

            // Transition resolve image
            transitionAttachment(
                resolveRef.attachment,
                resolveRef.layout,
                VK_IMAGE_ASPECT_COLOR_BIT
            );
        }
        else
        {
            colorAttachment.resolveMode =
                VK_RESOLVE_MODE_NONE;

            colorAttachment.resolveImageView =
                VK_NULL_HANDLE;

            colorAttachment.resolveImageLayout =
                VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }


    // ------------------------------------------------------------
    // DEPTH-ONLY ATTACHMENTS
    // ------------------------------------------------------------

    for (size_t i = 0; i < depthRefs.size(); ++i)
    {
        const auto& ref = depthRefs[i];

        a2[i].imageView =
            getImageView(ref.attachment);

        a2[i].imageLayout =
            ref.layout;

        transitionAttachment(
            ref.attachment,
            ref.layout,
            VK_IMAGE_ASPECT_DEPTH_BIT
        );
    }


    // ------------------------------------------------------------
    // STENCIL-ONLY ATTACHMENTS
    // ------------------------------------------------------------

    for (size_t i = 0; i < stencilRefs.size(); ++i)
    {
        const auto& ref = stencilRefs[i];

        a3[i].imageView =
            getImageView(ref.attachment);

        a3[i].imageLayout =
            ref.layout;

        transitionAttachment(
            ref.attachment,
            ref.layout,
            VK_IMAGE_ASPECT_STENCIL_BIT
        );
    }


    // ------------------------------------------------------------
    // COMBINED DEPTH/STENCIL ATTACHMENTS
    // ------------------------------------------------------------

    for (size_t i = 0; i < depthStencilRefs.size(); ++i)
    {
        const auto& ref = depthStencilRefs[i];

        a4[i].imageView =
            getImageView(ref.attachment);

        a4[i].imageLayout =
            ref.layout;

        transitionAttachment(
            ref.attachment,
            ref.layout,
            VK_IMAGE_ASPECT_DEPTH_BIT |
            VK_IMAGE_ASPECT_STENCIL_BIT
        );
    }


    // ------------------------------------------------------------
    // CONFIGURE DYNAMIC RENDERING
    // ------------------------------------------------------------

    renderingInfo.sType =
        VK_STRUCTURE_TYPE_RENDERING_INFO;

    renderingInfo.renderArea.offset = { 0, 0 };

    renderingInfo.renderArea.extent =
        swapchainmanager->GetExtent();

    renderingInfo.layerCount = 1;


    // Color attachments
    renderingInfo.colorAttachmentCount =
        static_cast<uint32_t>(a1.size());

    renderingInfo.pColorAttachments =
        a1.empty() ? nullptr : a1.data();


    // Depth and stencil attachments
    assert(a4.size() <= 1);
    assert(a2.size() <= 1);
    assert(a3.size() <= 1);

    if (!a4.empty())
    {
        // Combined D24S8 attachment
        assert(a2.empty() && a3.empty());

        renderingInfo.pDepthAttachment = &a4[0];
        renderingInfo.pStencilAttachment = &a4[0];
    }
    else
    {
        // Separate depth/stencil attachments
        renderingInfo.pDepthAttachment =
            a2.empty() ? nullptr : &a2[0];

        renderingInfo.pStencilAttachment =
            a3.empty() ? nullptr : &a3[0];
    }
    // ------------------------------------------------------------
    // BEGIN DYNAMIC RENDERING
    // ------------------------------------------------------------
    auto pipeline = pipelineManager.GetPipeline();
    vkCmdBeginRendering(commandBuffer, &renderingInfo);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineManager.GetPipeline());
    
    // Push constants
    const auto & range = pipelineManager.GetPushConstantRange(0);
    vkCmdPushConstants(
        commandBuffer,
        pipelineManager.GetPipelineLayout(),
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
    auto& vbuffer = square.vertexResource->Buffer;
    auto& ibuffer = square.indiceResource->Buffer;
        
    if (vbuffer != VK_NULL_HANDLE) {
           
        VkDeviceSize offsets[] = {square.vertexResource->Offset + square.vertexResource->GetAlignedDataSize(256) * imageIndex };

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vbuffer, offsets);
        vkCmdBindDescriptorSets(commandBuffer, 
                                VK_PIPELINE_BIND_POINT_GRAPHICS, 
                                pipelineManager.GetPipelineLayout(),
                                0, 
                                static_cast<uint32_t>(pipelineDescriptorSets[currentPipelineKey][imageIndex].size()),
                                pipelineDescriptorSets[currentPipelineKey][imageIndex].data(),
                                0, 
                                nullptr);

        if(ibuffer != VK_NULL_HANDLE){
            vkCmdBindIndexBuffer(commandBuffer, ibuffer, 0, VK_INDEX_TYPE_UINT16);
            uint16_t arrayCount = square.indiceResource->ArraySize;
            vkCmdDrawIndexed(commandBuffer, arrayCount, 1, 0, 0, 0);
      
        }
    }
    

    vkCmdEndRendering(commandBuffer);


    auto& swapchainLayout = layouts[swapchainIndex];

    TransitionImage(
        commandBuffer,
        swapchainImage,
        swapchainLayout,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_IMAGE_ASPECT_COLOR_BIT);


    // --------------------------------------------------
    // End command buffer
    // --------------------------------------------------

    result =vkEndCommandBuffer(commandBuffer);
     
}


void Renderer::RecreateSwapchain(){
    auto device = gpu->GetVkDevice();
    int width = 0, height = 0;

    vkDeviceWaitIdle(device);
    //this->framebufferContainer->Cleanup();
    swapchainmanager->Cleanup(device);
    
    swapchainmanager->RechooseSwapExtent();

    this->swapchainmanager->CreateSwapchain();
    //this->framebufferContainer->ReCreateFramebuffers();

    auto extent = swapchainmanager->GetExtent();
    frameinfo.iResolution = {extent.width, extent.height};
}

void Renderer::DestroyFrameBuffers() {

}
void Renderer::CreateFrameBuffers() {

}