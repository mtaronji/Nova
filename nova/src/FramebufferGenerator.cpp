#include "FramebufferGenerator.hpp"
#include "SwapchainManager.hpp"
#include "GPU.hpp"
#include "RenderPassManager.hpp"
#include "ImageOps.hpp"

FramebufferGenerator* FramebufferGenerator::Create(std::shared_ptr<GPU> gpu,
                            RenderPassManager* renderpassManager,
                            std::shared_ptr<SwapchainManager> swapchainManager){

    return new FramebufferGenerator(gpu,renderpassManager,swapchainManager);
}
FramebufferGenerator::  FramebufferGenerator(
                        std::shared_ptr<GPU> gpu,
                        RenderPassManager* renderpassManager,
                        std::shared_ptr<SwapchainManager> swapchainManager) : gpu(gpu),renderpassManager(renderpassManager), swapchainManager(swapchainManager)
{
    CreateRenderPassResources();
    CreateFramebuffers();
  
}

void FramebufferGenerator::CreateFramebuffers(){
    auto imageViews = swapchainManager->GetImageViews();
    auto extent = swapchainManager->GetExtent();
    this->framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); ++i) {
        
        //any attachment we specified as the present attachment will use the swapchain images and image views
        framebufferImageViewsPerFrame[i][swapchainImageViewIndex] = imageViews[i];
  
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderpassManager->GetRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(framebufferImageViewsPerFrame[i].size());
        framebufferInfo.pAttachments = framebufferImageViewsPerFrame[i].data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(gpu->GetVkDevice(), &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

//create our image view resources for the framebuffers
//each attachment needs an image view EXCEPT the swapchain image present attachment. -- this will use the swapchain image view
void FramebufferGenerator::CreateRenderPassResources(){
    
    auto& attachments = renderpassManager->GetAttachmentDescriptions();
    auto& imageUses = renderpassManager->GetImageUseFlags();
    auto& imageAspects = renderpassManager->GetImageAspectFlags();
    auto attachmentCount = attachments.size();
    assert(attachments.size() == imageUses.size()); 
    assert(attachments.size() == imageAspects.size());

    auto maxframes = swapchainManager->GetImageCount();
    auto extent = swapchainManager->GetExtent();
    

    for(int frame = 0; frame < maxframes; frame++){
        std::vector<VkImage> attachmentImagesPerFrame(attachmentCount);
        std::vector<VkDeviceMemory> attachmentImageMemoryPerFrame(attachmentCount);
        std::vector<VkImageView> attachmentImageViewsPerFrame(attachmentCount);
        for(int i = 0; i < attachments.size(); i++){
            if(attachments[i].finalLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR){
                swapchainImageViewIndex = i;
                continue;
            }
            VkImage image = VK_NULL_HANDLE;
            VkDeviceMemory imageMemory = VK_NULL_HANDLE;
            VkImageView imageView = VK_NULL_HANDLE;
            ImageOps::CreateImage(  gpu.get(),
                                        extent.width,
                                        extent.height, 
                                        1,
                                        1,
                                        attachments[i].samples,
                                        attachments[i].format,
                                        VK_IMAGE_TILING_OPTIMAL,    
                                        imageUses[i],    
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        VK_IMAGE_TYPE_2D,
                                        VK_IMAGE_LAYOUT_UNDEFINED,
                                        0,
                                        image,     
                                        imageMemory                      
                                    );
    
            ImageOps::CreateImageView(gpu.get(),image, attachments[i].format, imageAspects[i], VK_IMAGE_VIEW_TYPE_2D, 1, 1, imageView);
            attachmentImagesPerFrame[i] = image;
            attachmentImageMemoryPerFrame[i] = imageMemory;
            attachmentImageViewsPerFrame[i] = imageView;
        }
        framebufferImageViewsPerFrame.push_back(attachmentImageViewsPerFrame);
        framebufferImagesPerFrame.push_back(attachmentImagesPerFrame);
        framebufferImageMemoryPerFrame.push_back(attachmentImageMemoryPerFrame);
    }
    if(swapchainImageViewIndex == NOT_SET){throw std::runtime_error("unable to find the swapchainattachment for the framebuffer. Fatal error");}
}
void FramebufferGenerator::Cleanup() {
    vkDeviceWaitIdle(gpu->GetVkDevice());
    for (auto framebuffer : this->framebuffers) {
        vkDestroyFramebuffer(gpu->GetVkDevice(), framebuffer, nullptr);
    }
    framebuffers.clear();

    for(auto& imageviewsPerFrame : framebufferImageViewsPerFrame){
        for(auto& imageView: imageviewsPerFrame){
            
            vkDestroyImageView(gpu->GetVkDevice(), imageView, nullptr);
        }
    }
    framebufferImageViewsPerFrame.clear();
    for(auto& imagesPerFrame : framebufferImagesPerFrame){
        for(auto& image: imagesPerFrame){
            vkDestroyImage(gpu->GetVkDevice(), image, nullptr);
        }
    }
    framebufferImagesPerFrame.clear();
    for(auto& memoryPerFrame : framebufferImageMemoryPerFrame){
        for(auto& memory: memoryPerFrame){
            vkFreeMemory(gpu->GetVkDevice(), memory, nullptr);
        }
    }
    framebufferImageMemoryPerFrame.clear();

}

void FramebufferGenerator::ReCreateFramebuffers(){
    FramebufferGenerator::CreateRenderPassResources();
    FramebufferGenerator::CreateFramebuffers();
}
