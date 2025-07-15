#include "ImageResource.hpp"

ImageResource::ImageResource(VkImageUsageFlags usage, VkImageLayout layout, uint32_t copies, VkMemoryPropertyFlags memoryProperties)
    :Resource(copies,memoryProperties), Usage(usage), ImageLayout(layout) {

    bool hasMoreThanOneBitSet = (usage & (usage - 1)) != 0;
    assert(!hasMoreThanOneBitSet && "Only one type of usage should be specified for an image resource"); 
   
}

void ImageResource::Upload(void* srcData, VkDeviceSize dataSize) {

    if (!Data) {
        // First allocation only
        Data = malloc(dataSize);
        if (!Data) throw std::bad_alloc();
    }
    memcpy(Data, srcData, dataSize);

}


void ImageResource::Cleanup(GPU* gpu) {

    if (Data) {
        free(Data);
        Data = nullptr;
    }
}