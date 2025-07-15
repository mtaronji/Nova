#include "ImageOps.hpp"

void ImageOps::CreateImage(
    std::shared_ptr<GPU> gpu,
    uint32_t width,
    uint32_t height,
    uint32_t mipLevels,
    uint32_t arrayLayers,
    VkSampleCountFlagBits samples,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImageType imageType,
    VkImageLayout initialLayout,
    VkImageCreateFlags createFlags,
    VkImage& image,
    VkDeviceMemory& imageMemory
) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = imageType;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = initialLayout;
    imageInfo.usage = usage;
    imageInfo.samples = samples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = createFlags;

    if (vkCreateImage(gpu->GetVkDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(gpu->GetVkDevice(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(gpu->GetPhysicalDevice(), &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            allocInfo.memoryTypeIndex = i;
            break;
        }
    }

    auto result = vkAllocateMemory(gpu->GetVkDevice(), &allocInfo, nullptr, &imageMemory);
    assert(result == VK_SUCCESS && "Create image failed");

    result = vkBindImageMemory(gpu->GetVkDevice(), image, imageMemory, 0);
    assert(result == VK_SUCCESS && "Bind Image Memory failed");
}

void ImageOps::CreateImageView(
    std::shared_ptr<GPU> gpu,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspectFlags,
    VkImageViewType viewType,
    uint32_t mipLevels,
    uint32_t arrayLayers,
    VkImageView& imageView
) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = viewType;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = arrayLayers;

    auto result = vkCreateImageView(gpu->GetVkDevice(), &viewInfo, nullptr, &imageView);
    assert(result == VK_SUCCESS && "Create image View failed");
}

void ImageOps::CreateSampler(
        std::shared_ptr<GPU> gpu, 
        uint32_t mipLevels, 
        VkFilter magFilter,
        VkFilter minFilter,
        VkSamplerMipmapMode mipmapmode,
        VkSamplerAddressMode addressModeU,
        VkSamplerAddressMode addressNodeV,
        VkSamplerAddressMode addressNodeW,
        float mipLodBias,
        VkBool32 anisotrophyEnable,
        float maxAnisotrophy,
        float minLod,
        float maxLod,
        VkBorderColor borderColor,
        VkBool32 unnormalizedCoordinates,
        VkSampler& sampler
    ) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = magFilter;
    samplerInfo.minFilter = minFilter;
    samplerInfo.mipmapMode = mipmapmode;
    samplerInfo.addressModeU = addressModeU;
    samplerInfo.addressModeV = addressNodeV;
    samplerInfo.addressModeW = addressNodeW;
    samplerInfo.mipLodBias = mipLodBias;
    samplerInfo.anisotropyEnable = anisotrophyEnable;
    samplerInfo.maxAnisotropy = maxAnisotrophy;
    samplerInfo.minLod = minLod;
    samplerInfo.maxLod = maxLod;
    samplerInfo.borderColor = borderColor;
    samplerInfo.unnormalizedCoordinates = unnormalizedCoordinates;

    auto result = vkCreateSampler(gpu->GetVkDevice(), &samplerInfo, nullptr, &sampler);
    assert(result == VK_SUCCESS && "Create image Sampler failed");
    
}
