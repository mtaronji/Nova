#include "Resource.hpp"

void Resource::Upload(void* srcData, VkDeviceSize dataSize, uint32_t arraySize) {

    if (!data) {
        // First allocation only
        data = malloc(dataSize);
        assert(data && "failed to upload data");
    }

    memcpy(data, srcData, dataSize);
    this->previousSize = this->dataSize;
    this->dataSize = dataSize;
    this->arraySize = arraySize;
}