#include "FramebufferGenerator.hpp"
#include "SwapchainManager.hpp"
#include "GPU.hpp"
#include "RenderPassManager.hpp"
#include "ImageOps.hpp"

FramebufferGenerator FramebufferGenerator::Create(std::shared_ptr<GPU> gpu,
                            const RenderPassManager&  renderpassManager,
                            std::shared_ptr<SwapchainManager> swapchainManager){

    return FramebufferGenerator(gpu,renderpassManager,swapchainManager);
}
FramebufferGenerator::  FramebufferGenerator(
                        std::shared_ptr<GPU> gpu,
                        const RenderPassManager& renderpassManager,
                        std::shared_ptr<SwapchainManager> swapchainManager):gpu(gpu)
{
    CreateRenderPassResources(gpu, renderpassManager,swapchainManager);
  
  
}

//create our image view resources for the framebuffers
//each attachment needs an image view EXCEPT the swapchain image present attachment. -- this will use the swapchain image view
void FramebufferGenerator::CreateRenderPassResources(
    std::shared_ptr<GPU> gpu,
    const RenderPassManager& renderpassManager,
    std::shared_ptr<SwapchainManager> swapchainManager){
    
    auto& attachments = renderpassManager.GetAttachmentDescriptions();
    auto& imageUses = renderpassManager.GetImageUseFlags();
    auto& imageAspects = renderpassManager.GetImageAspectFlags();
    auto attachmentCount = attachments.size();
    assert(attachments.size() == imageUses.size()); 
    assert(attachments.size() == imageAspects.size());

    auto maxframes = swapchainManager->GetImageCount();
    auto extent = swapchainManager->GetExtent();
    

    for(int frame = 0; frame < maxframes; frame++){
        std::vector<VkImage> attachmentImagesPerFrame(attachmentCount);
        std::vector<VkDeviceMemory> attachmentImageMemoryPerFrame(attachmentCount);
        std::vector<VkImageView> attachmentImageViewsPerFrame(attachmentCount);
        std::vector<VkImageLayout> currentImageLayoutsForAttachments(attachmentCount, VK_IMAGE_LAYOUT_UNDEFINED);
        for(int i = 0; i < attachments.size(); i++){
            
            VkImage image = VK_NULL_HANDLE;
            VkDeviceMemory imageMemory = VK_NULL_HANDLE;
            VkImageView imageView = VK_NULL_HANDLE;
            ImageOps::CreateImage(  gpu,
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
    
            ImageOps::CreateImageView(gpu,image, attachments[i].format, imageAspects[i], VK_IMAGE_VIEW_TYPE_2D, 1, 1, imageView);
            attachmentImagesPerFrame[i] = image;
            attachmentImageMemoryPerFrame[i] = imageMemory;
            attachmentImageViewsPerFrame[i] = imageView;
        }
        framebufferImageViewsPerFrame.push_back(attachmentImageViewsPerFrame);
        framebufferImagesPerFrame.push_back(attachmentImagesPerFrame);
        framebufferImageMemoryPerFrame.push_back(attachmentImageMemoryPerFrame);
        currentImageLayoutsPerFrame.push_back(currentImageLayoutsForAttachments);
    }
}
void FramebufferGenerator::Cleanup() {

    vkDeviceWaitIdle(gpu->GetVkDevice());
  
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


