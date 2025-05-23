#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "NovaConstants.hpp"

class GPU;
class SwapchainManager;
class RenderPassManager;
class FramebufferGenerator {
    public:
        FramebufferGenerator() = default;

        FramebufferGenerator(std::shared_ptr<GPU> gpu,
                            std::shared_ptr<RenderPassManager> renderpassManager,
                            std::shared_ptr<SwapchainManager> swapchainManager);

        void CreateFramebuffers();
        void ReCreateFramebuffers();
        void CreateRenderPassResources();
        void Cleanup();

        std::vector<VkFramebuffer> GetFramebuffers() const { return framebuffers; }

    protected:
        std::shared_ptr<GPU> gpu;
        std::shared_ptr<RenderPassManager> renderpassManager;
        std::shared_ptr<SwapchainManager> swapchainManager;

        VkFormat depthFormat;
        VkSampleCountFlagBits msaaSamples;

        std::vector<VkFramebuffer> framebuffers;

        std::vector<VkImage> colorResolveImages;
        std::vector<VkDeviceMemory> colorResolveMemories;
        std::vector<VkImageView> colorResolveViews;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthMemories;
        std::vector<VkImageView> depthViews;

        std::vector<std::vector<VkImage>> framebufferImagesPerFrame;
        std::vector<std::vector<VkImageView>> framebufferImageViewsPerFrame;
        std::vector<std::vector<VkDeviceMemory>> framebufferImageMemoryPerFrame;
        uint32_t swapchainImageViewIndex = NOT_SET;

};