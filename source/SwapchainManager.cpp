#include "SwapchainManager.hpp"

// Image type (2D, 3D, cube map, etc.)
//     // Format (RGBA, depth format, etc.)
//     // Mip level (which level of the texture to sample from)
//     // Layer or array layers (which specific layers in a 2D array or cube map to sample)
//     // Aspect (color, depth, or stencil)
//     // Without an VkImageView, Vulkan wouldn't know how to interpret the image data in the context of a specific rendering operation.
void SwapchainManager::CreateSwapchain() {
    
    uint32_t imageCount = details.capabilities.minImageCount + 1;
    if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
        imageCount = details.capabilities.maxImageCount;
    }
    this->imageCount = imageCount;
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = engine->GetSurface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = gpu->FindQueueFamilies();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = details.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = this->presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(gpu->GetVkDevice(), &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(gpu->GetVkDevice(), swapchain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(gpu->GetVkDevice(), swapchain, &imageCount, images.data());

    this->imageFormat = surfaceFormat.format;
    this->extent = extent;
    //create image views now
    CreateImageViews(gpu->GetVkDevice());
}

SwapchainManager::~SwapchainManager(){
    
}

SwapchainManager::SwapchainManager(std::shared_ptr<GPU> gpu, 
                                   std::shared_ptr<Shell> shell,
                                   std::shared_ptr<VulkanEngine> engine,
                                   SwapChainSupportDetails details,
                                   VkSurfaceFormatKHR surfaceFormat,
                                   VkPresentModeKHR presentMode,
                                   VkExtent2D extent)
                                   :gpu(gpu), shell(shell),engine(engine), details(details),surfaceFormat(surfaceFormat),presentMode(presentMode),extent(extent){
    
}
SwapchainManager::Builder& SwapchainManager::Builder::WithGPU(std::shared_ptr<GPU> gpu){
    this->gpu = gpu;
    return *this;
}
SwapchainManager::Builder& SwapchainManager::Builder::WithEngine(std::shared_ptr<VulkanEngine> engine){
    this->engine = engine;
    return *this;
}
SwapchainManager::Builder& SwapchainManager::Builder::WithShell(std::shared_ptr<Shell> shell){
    this->shell = shell;
    return *this;
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

void SwapchainManager::Builder::ChooseSwapSurfaceFormat() {
    for (const auto& availableFormat : this->details.formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            this->surfaceFormat = availableFormat;
            return;
        }
    }

    this->surfaceFormat = this->details.formats[0];
    return;
}

void SwapchainManager::Builder::ChooseSwapPresentMode() {

    for (const auto& availablePresentMode : this->details.presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            this->presentMode = availablePresentMode;
            return;
        }
    }

    this->presentMode = VK_PRESENT_MODE_FIFO_KHR;

}
    
void SwapchainManager::Builder::ChooseSwapExtent() {

    auto capabilities = this->details.capabilities;
    auto window = shell->GetWindow();
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        this->extent = capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        this->extent = actualExtent;
   
    }
}

void SwapchainManager::RechooseSwapExtent(){
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu->GetPhysicalDevice(), engine->GetSurface(), &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->GetPhysicalDevice(), engine->GetSurface(), &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->GetPhysicalDevice(), engine->GetSurface(), &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->GetPhysicalDevice(), engine->GetSurface(), &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->GetPhysicalDevice(), engine->GetSurface(), &presentModeCount, details.presentModes.data());
    }
    auto capabilities = this->details.capabilities;
    auto window = shell->GetWindow();
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        this->extent = capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        this->extent = actualExtent;
   
    }
}
    
void  SwapchainManager::Builder::QuerySwapChainSupport() {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu->GetPhysicalDevice(), engine->GetSurface(), &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->GetPhysicalDevice(), engine->GetSurface(), &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->GetPhysicalDevice(), engine->GetSurface(), &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->GetPhysicalDevice(), engine->GetSurface(), &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->GetPhysicalDevice(), engine->GetSurface(), &presentModeCount, details.presentModes.data());
    }

    this->details = details;
}

std::shared_ptr<SwapchainManager> SwapchainManager::Builder::Build(){
    QuerySwapChainSupport();
    ChooseSwapExtent();
    ChooseSwapPresentMode();
    ChooseSwapSurfaceFormat();

    auto surface = engine->GetSurface();
    std::shared_ptr<SwapchainManager> manager = std::make_shared<SwapchainManager>(gpu, shell, engine, details, surfaceFormat, presentMode, extent);
    
    return manager;
}
void SwapchainManager::Cleanup(VkDevice device) {
    vkDeviceWaitIdle(device);

    // for (auto view : imageViews)                                //framebuffer manager cleans these up
    //     vkDestroyImageView(device, view, nullptr);
    imageViews.clear();

    if (swapchain)
        vkDestroySwapchainKHR(device, swapchain, nullptr);
}