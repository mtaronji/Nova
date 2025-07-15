#pragma once

#include "Resource.hpp"
#include <vulkan/vulkan.hpp>

class ImageResource : public Resource {
public:
    ImageResource() = delete;
    ImageResource(VkImageUsageFlags usage, VkImageLayout layout, uint32_t copies, VkMemoryPropertyFlags memoryProperties);
    ~ImageResource() override = default;

    void Upload(void* srcData, VkDeviceSize dataSize);
    void Cleanup(GPU* gpu) override;

    VkImageView ImageView = VK_NULL_HANDLE;
    VkImageUsageFlags Usage;
    VkImage Image = VK_NULL_HANDLE;
    VkImageLayout ImageLayout;

protected:
  
};