#include "BufferResource.hpp"
#include "BufferOps.hpp"
#include "GPU.hpp"
#include <stdexcept>
#include <cstring>


BufferResource::BufferResource(VkBufferUsageFlags usage, uint32_t copies, uint32_t set, uint32_t binding)
:Resource(copies,set,binding), usage(usage){

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

    kind = ResourceKind::Buffer;
}


size_t BufferResource::GetAlignedDataSize(VkDeviceSize alignment){
    return BufferOps::AlignUp(dataSize, alignment);
}

uint32_t BufferResource::GetArraySize() const {
    if (arraySize == NOT_SET) {
        throw std::runtime_error("ArraySize has no been set. Fatal Error");
    }
    else {
        return arraySize;
    }
}
void BufferResource::Cleanup(GPU* gpu){

    if(data){
        free(data);
        data = nullptr;
    }      
}