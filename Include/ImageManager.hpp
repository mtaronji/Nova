#pragma once
#include <vulkan/vulkan.hpp>

class GPU;
class ImageManager{

    ImageManager() = delete;
    ImageManager(const ImageManager&) = delete;

    public:
        static void CreateImage(GPU* gpu,
                                uint32_t width,
                                uint32_t height,
                                VkSampleCountFlagBits numSamples,
                                VkFormat format,
                                VkImageTiling tiling,
                                VkImageUsageFlags usage,
                                VkMemoryPropertyFlags properties,
                                VkImage& image,
                                VkDeviceMemory& imageMemory);

        static void CreateImageView(GPU *gpu, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& imageView);
};