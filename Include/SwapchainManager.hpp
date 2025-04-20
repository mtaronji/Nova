#pragma once

#include <vulkan/vulkan.h>
#include "GPU.hpp"
#include <stdexcept>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
class SwapchainManager {
    public:
        SwapchainManager() = default;
        void CreateSwapchain( GPU& device,
            SwapChainSupportDetails& supportDetails,
            VkSurfaceFormatKHR& surfaceFormat,
            VkPresentModeKHR& presentMode,
            VkExtent2D& extent,     
            VkSurfaceKHR& surface);

        void Cleanup(VkDevice device);

        VkSwapchainKHR GetSwapchain() const { return swapchain; }
        const std::vector<VkImageView>& GetImageViews() const { return imageViews; }
        VkFormat GetImageFormat() const { return imageFormat; }
        VkExtent2D GetExtent() const { return extent; }

      

        class Builder{
            public:
                Builder() = default;
                Builder& SetGPU(GPU& gpu);
                Builder& SetVulkanEngine(VulkanEngine &engine);
                Builder& QuerySwapChainSupport();
                Builder& ChooseSwapSurfaceFormat();   //depends on query swap chain support
                Builder& ChooseSwapPresentMode();     //depends on query swapchain support
                Builder& ChooseSwapExtent();
                SwapchainManager Build();
                

            private:
                GPU* gpu;
                SwapChainSupportDetails swapChainSupportDetails;
                VkSurfaceFormatKHR surfaceFormat;
                VkPresentModeKHR presentMode;
                VkExtent2D extent;        
                VulkanEngine* engine;
            
        };

    protected:
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
        VkFormat imageFormat;
        VkExtent2D extent;
        std::vector<VkImageView> swapChainImageViews;

        void CreateImageViews(VkDevice device);
};
