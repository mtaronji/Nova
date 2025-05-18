#include "BufferResource.hpp"
#include <cstring>

BufferResource::BufferResource(VkBufferUsageFlags usage, uint32_t set, uint32_t binding):usage(usage), buffer(VK_NULL_HANDLE), set(set), binding(binding){

    bool hasMoreThanOneBitSet = (usage & (usage - 1)) != 0;
    if(hasMoreThanOneBitSet){throw std::runtime_error("BufferResources should have only 1 usage specified");}

    if((usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)      && ((set == NOT_SET) || (binding == NOT_SET))){
        throw std::runtime_error("for a uniform, you need to specify it's binding position as well as the index for it's descriptor set");}
    if((usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)      && ((set == NOT_SET) || (binding == NOT_SET))){
        throw std::runtime_error("for a storage, you need to specify it's binding position as well as the index for it's descriptor set");}
    if((usage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT)&& ((set == NOT_SET) || (binding == NOT_SET))){
        throw std::runtime_error("for a uniform texel, you need to specify it's binding position as well as the index for it's descriptor set");}
    if((usage & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT)&& ((set == NOT_SET) || (binding == NOT_SET))){
        throw std::runtime_error("for a storage texel, you need to specify it's binding position as well as the index for it's descriptor set");}   
}

void BufferResource::Upload(void* srcData, VkDeviceSize dataSize, uint32_t arraySize) {
    data = malloc(dataSize);
    memcpy(data, srcData, dataSize);
    this->previousSize = this->dataSize;
    this->dataSize = dataSize;
    this->arraySize = arraySize;
    this->offset = offset;
    
}
