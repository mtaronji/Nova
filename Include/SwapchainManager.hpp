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
            Builder& QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
            Builder& ChooseSwapSurfaceFormat();   
            Builder& ChooseSwapPresentMode();
            Builder& ChooseSwapExtent(GLFWwindow* window);

            private:
                SwapChainSupportDetails swapChainSupport;
                VkSurfaceFormatKHR surfaceFormat;
                VkPresentModeKHR presentMode;
                VkExtent2D extent;        
                VkSurfaceKHR surface;
            
        };

    private:
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
        VkFormat imageFormat;
        VkExtent2D extent;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        void CreateImageViews(VkDevice device);
};


//         SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
//         VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
//         VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
//         VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);