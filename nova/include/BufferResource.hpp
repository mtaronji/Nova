#pragma once

#include "NovaConstants.hpp"
#include "Resource.hpp"
#include <vulkan/vulkan.hpp>
#include <memory>

class GPU;

//this is for resources that go in buffers
class BufferResource : public Resource {
public:
    BufferResource(VkBufferUsageFlags usage, uint32_t copies, VkMemoryPropertyFlags memoryProperties);
    BufferResource(const BufferResource&) = delete;

    BufferResource& operator=(const BufferResource&) = delete;

    ~BufferResource() override = default;
    
    void Upload(void* srcData, VkDeviceSize dataSize, uint32_t arraySize = NOT_SET);
    void Cleanup(GPU* gpu) override;

    VkBuffer Buffer = VK_NULL_HANDLE;
    size_t PreviousSize = 0;
    uint32_t ArraySize = NOT_SET;  //for indice data
    VkBufferUsageFlags Usage;
    VkDeviceSize Offset = 0;

protected:

private:
   

};