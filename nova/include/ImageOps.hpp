#pragma once
#include <vulkan/vulkan.hpp>
#include "GPU.hpp"


class ImageOps {

    ImageOps() = delete;
    ImageOps(const ImageOps&) = delete;

    public:
        static void CreateImage(
            GPU* gpu,
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
        );

        static void CreateImageView(
            GPU* gpu,
            VkImage image,
            VkFormat format,
            VkImageAspectFlags aspectFlags,
            VkImageViewType viewType,
            uint32_t mipLevels,
            uint32_t arrayLayers,
            VkImageView& imageView
        );

        static void CreateSampler(
            GPU* gpu,
            uint32_t mipLevels,
            VkSampler& sampler
        );
};