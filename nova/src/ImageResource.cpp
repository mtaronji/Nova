#include "ImageResource.hpp"

ImageResource::ImageResource(VkImageUsageFlags usage, uint32_t copies, uint32_t set, uint32_t binding)
    :Resource(copies, set, binding), usage(usage) {

    bool hasMoreThanOneBitSet = (usage & (usage - 1)) != 0;
    if (hasMoreThanOneBitSet) { throw std::runtime_error("BufferResources should have only 1 usage specified"); }

    if ((usage & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) && ((set == NOT_SET) || (binding == NOT_SET))) {
        throw std::runtime_error("for a input attachment, you need to specify it's binding position as well as the index for it's descriptor set");
    }
    if ((usage & VK_IMAGE_USAGE_STORAGE_BIT) && ((set == NOT_SET) || (binding == NOT_SET))) {
        throw std::runtime_error("for a image storage bit, you need to specify it's binding position as well as the index for it's descriptor set");
    }
    if ((usage & VK_IMAGE_USAGE_SAMPLED_BIT) && ((set == NOT_SET) || (binding == NOT_SET))) {
        throw std::runtime_error("for an image Sampled bit, you need to specify it's binding position as well as the index for it's descriptor set");
    }

    kind = ResourceKind::Image;
}

void ImageResource::Upload(void* srcData, VkDeviceSize dataSize) {

    if (!data) {
        // First allocation only
        data = malloc(dataSize);
        if (!data) throw std::bad_alloc();
    }
    memcpy(data, srcData, dataSize);

}


void ImageResource::Cleanup(GPU* gpu) {

    if (data) {
        free(data);
        data = nullptr;
    }
}