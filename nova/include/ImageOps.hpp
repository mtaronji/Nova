#pragma once
#include <vulkan/vulkan.hpp>
#include <memory>

class CommandManager;
class GPU;
class ImageOps {

    ImageOps() = delete;
    ImageOps(const ImageOps&) = delete;
  
    public:
        static VkBuffer stagingBuffer;
        static VkDeviceMemory stagingMemory;
        static VkDeviceSize currentStagingOffset;
        static VkDeviceSize currentStagingAllocationSize;

        static void CreateImage(
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
        );

        static void CreateImageView(
            std::shared_ptr<GPU> gpu,
            VkImage image,
            VkFormat format,
            VkImageAspectFlags aspectFlags,
            VkImageViewType viewType,
            uint32_t mipLevels,
            uint32_t arrayLayers,
            VkImageView& imageView
        );

        static void CreateSampler(std::shared_ptr<GPU> gpu, 
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
        );

        static void UploadToStaging(
            std::shared_ptr<GPU> gpu,
            VkDeviceSize imageSize,
            const uint8_t* pixels);

        static void TransitionImageLayout(
            std::shared_ptr<GPU> gpu,
            std::shared_ptr<CommandManager> cmdManager,
            VkImage& image,
            VkFormat format,
            VkImageLayout currentLayout,
            VkImageLayout requestedLayout,
            uint32_t mipLevels,
            uint32_t layerCount);

        //only for tightly packed image data in the staging buffer
        static void CopyBufferToImage(
            std::shared_ptr<CommandManager> cmdManager,
            VkBuffer buffer,
            VkDeviceSize stagingBufferOffset,
            VkImage image,
            VkImageAspectFlags aspectMask,
            uint32_t width,
            uint32_t height);
};