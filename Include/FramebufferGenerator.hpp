#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include "SwapchainManager.hpp"

class FramebufferGenerator {
    public:
        FramebufferGenerator() = default;

        FramebufferGenerator(VkDevice device,
            VkRenderPass renderPass,
            std::shared_ptr<SwapchainManager> swapchainManager);

        std::vector<VkFramebuffer> GetFramebuffers() const{return framebuffers;}
        void CreateFrameBuffers(VkDevice device, VkRenderPass renderPass, std::shared_ptr<SwapchainManager> swapchainManager);
        void ReCreateFrameBuffers(VkDevice device, VkRenderPass renderPass, std::shared_ptr<SwapchainManager> swapchainManager);

        void Cleanup(const VkDevice& device);

        

    protected:
        std::vector<VkFramebuffer> framebuffers;
       
        
};