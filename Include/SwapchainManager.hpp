#pragma once

#include <vulkan/vulkan.h>
#include "GPU.hpp"
#include <stdexcept>

class SwapchainManager {
public:
    void Create(GPU& device, VkSurfaceKHR surface);
    void Cleanup(VkDevice device);

    VkSwapchainKHR GetSwapchain() const { return swapchain; }
    const std::vector<VkImageView>& GetImageViews() const { return imageViews; }
    VkFormat GetImageFormat() const { return imageFormat; }
    VkExtent2D GetExtent() const { return extent; }

private:
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    VkFormat imageFormat;
    VkExtent2D extent;

    void CreateImageViews(VkDevice device);
};