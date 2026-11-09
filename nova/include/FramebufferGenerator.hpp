#pragma once
#include <stdexcept>
#include "NovaConstants.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <cassert>

class GPU;
class SwapchainManager;
class RenderPassManager;
class ImageManager;

class FramebufferGenerator {
    public:
        FramebufferGenerator() = delete;
 
        // Move
        FramebufferGenerator(FramebufferGenerator&&) noexcept = default;
        FramebufferGenerator& operator=(FramebufferGenerator&&) noexcept = default;

        // Don't allow copies
        FramebufferGenerator(const FramebufferGenerator&) = delete;
        FramebufferGenerator& operator=(const FramebufferGenerator&) = delete;
        static FramebufferGenerator Create(std::shared_ptr<GPU> gpu,
                            const RenderPassManager& renderpassManager,
                            std::shared_ptr<SwapchainManager> swapchainManager);

       
        void CreateRenderPassResources(std::shared_ptr<GPU> gpu,
            const RenderPassManager& renderpassManager,
            std::shared_ptr<SwapchainManager> swapchainManager);
        virtual void Cleanup();

        const std::vector<VkImageView>& GetImageViewsForFrame(int frame)  { return framebufferImageViewsPerFrame[frame]; }
        const std::vector<VkImage>& GetImagesForFrame(int frame) { return framebufferImagesPerFrame[frame]; }
        std::vector<VkImageLayout>& GetCurrentLayoutsForFrame(int frame)  { return currentImageLayoutsPerFrame[frame]; }
		uint32_t GetSwapchainAttachmentIndex() const { assert(swapchainAttachmentIndex != NOT_SET); return swapchainAttachmentIndex; }

    protected:
        FramebufferGenerator(std::shared_ptr<GPU> gpu,
                            const RenderPassManager& renderpassManager,
                            std::shared_ptr<SwapchainManager> swapchainManager);
                            

        std::vector<std::vector<VkImage>> framebufferImagesPerFrame;
        std::vector<std::vector<VkImageView>> framebufferImageViewsPerFrame;
        std::vector<std::vector<VkDeviceMemory>> framebufferImageMemoryPerFrame;
        std::vector<std::vector<VkImageLayout>> currentImageLayoutsPerFrame;
        std::shared_ptr<GPU> gpu;
        uint32_t swapchainAttachmentIndex = NOT_SET;

};