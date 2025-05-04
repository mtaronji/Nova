#pragma once

#include <vulkan/vulkan.h>
#include "GPU.hpp"
#include "Shell.hpp"
#include <stdexcept>
#include <memory>

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
                Builder& WithGPU(std::shared_ptr<GPU> gpu);
                Builder& WithEngine(std::shared_ptr<VulkanEngine> engine);
                Builder& WithShell(std::shared_ptr<Shell> shell);
                
                void QuerySwapChainSupport();
                void ChooseSwapSurfaceFormat();   //depends on query swap chain support
                void ChooseSwapPresentMode();     //depends on query swapchain support
                void ChooseSwapExtent();
                std::shared_ptr<SwapchainManager> Build();
                

            private:
                std::shared_ptr<GPU> gpu;
                std::shared_ptr<VulkanEngine> engine;
                std::shared_ptr<Shell> shell;
                SwapChainSupportDetails swapChainSupportDetails;
                VkSurfaceFormatKHR surfaceFormat;
                VkPresentModeKHR presentMode;
                VkExtent2D extent;        
            
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
