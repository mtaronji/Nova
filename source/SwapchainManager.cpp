#include "SwapchainManager.hpp"


void SwapchainManager::Create(GPU& device, VkSurfaceKHR surface) {
    // NOTE: This is placeholder logic. Normally, you'd query swapchain support details.
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = 2;
    createInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    createInfo.imageExtent = { 800, 600 };
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device.GetVkDevice(), &createInfo, nullptr, &swapchain) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swapchain");

    imageFormat = createInfo.imageFormat;
    extent = createInfo.imageExtent;

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device.GetVkDevice(), swapchain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device.GetVkDevice(), swapchain, &imageCount, images.data());

    CreateImageViews(device.GetVkDevice());
}

void SwapchainManager::CreateImageViews(VkDevice device) {
    imageViews.resize(images.size());

    for (size_t i = 0; i < images.size(); ++i) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = imageFormat;
        viewInfo.components = {};
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &viewInfo, nullptr, &imageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create image view");
    }
}

void SwapchainManager::Cleanup(VkDevice device) {
    for (auto view : imageViews)
        vkDestroyImageView(device, view, nullptr);
    imageViews.clear();

    if (swapchain)
        vkDestroySwapchainKHR(device, swapchain, nullptr);
}