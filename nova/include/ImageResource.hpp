#pragma once

#include "Resource.hpp"
#include <vulkan/vulkan.hpp>

class ImageResource : public Resource {
public:
    ImageResource(VkImageUsageFlags usage,uint32_t copies, uint32_t set, uint32_t binding);
    ~ImageResource() override = default;
    void Upload(void* srcData, VkDeviceSize dataSize);
    void Cleanup(GPU* gpu) override;

    // Add GetImageView, GetSampler, etc.
protected:
    VkImageView imageView = VK_NULL_HANDLE;
    VkImageUsageFlags usage;
};