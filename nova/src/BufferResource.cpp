#include "BufferResource.hpp"
#include "BufferOps.hpp"
#include "GPU.hpp"
#include <stdexcept>
#include <cstring>


BufferResource::BufferResource(std::shared_ptr<GPU> gpu,VkBufferUsageFlags usage, uint32_t copies, VkMemoryPropertyFlags memoryProperties)
:Resource(gpu,copies,memoryProperties), Usage(usage){

    bool hasMoreThanOneBitSet = (usage & (usage - 1)) != 0;
    assert(!hasMoreThanOneBitSet && "Only one buffer usage should be specified");
    this->resourceType = ResourceType::BUFFER;
}
void BufferResource::Upload(void* srcData, VkDeviceSize dataSize, uint32_t arraySize) {

    if (!Data) {
        // First allocation only
        Data = malloc(dataSize);
        assert(Data && "failed to upload data");
    }

    memcpy(Data, srcData, dataSize);
    this->PreviousSize = this->DataSize;
    this->DataSize = dataSize;
    this->ArraySize = arraySize;
}


void BufferResource::Cleanup(){

    if(Data){
        free(Data);
        Data = nullptr;
    }      
}

