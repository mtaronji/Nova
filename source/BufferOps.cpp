#include <vulkan/vulkan.h>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <cstring>
#include "CommandManager.hpp"
#include "BufferOps.hpp"

void BufferOps::CreateBuffer(GPU gpu, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {

    auto device = gpu.GetVkDevice();
    auto physicalDevice = gpu.GetPhysicalDevice();
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = size;
    allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void BufferOps::CopyBuffer(GPU gpu, CommandManager& commandManager, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

    VkCommandBuffer commandBuffer = commandManager.BeginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    commandManager.EndSingleTimeCommands(commandBuffer);
}

void BufferOps::CreateOnGPUBuffer(
    GPU gpu,
    CommandManager& commandManager,
    void* data,
    VkDeviceSize datasize,
    VkBufferUsageFlags usage,
    VkBuffer& buffer,
    VkDeviceMemory& memory){

    assert(datasize != 0);
    VkDeviceSize memorySize = datasize;

    auto paddedSize = BufferOps::AlignUp(memorySize, 256);  
    auto device = gpu.GetVkDevice();
    
    //we gonna pad it up to a multiple of 256
   
    // Create staging buffer (host-visible)
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    CreateBuffer(
        gpu,
        paddedSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,  
        stagingBuffer,
        stagingMemory
    );

    // Upload data
    void* pGpuMemory;
    vkMapMemory(device, stagingMemory, 0, paddedSize, 0, &pGpuMemory);  //mapping gpu memory that the cpu can see
    memcpy(pGpuMemory, data, (size_t)memorySize);

    VkMappedMemoryRange flushRange = {};
    flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    flushRange.memory = stagingMemory;       // The memory to flush/invalidate
    flushRange.offset = 0;                  // Start from the beginning
    flushRange.size = paddedSize;       

    vkFlushMappedMemoryRanges(device,1, &flushRange); 
    vkUnmapMemory(device, stagingMemory);
    pGpuMemory = nullptr;

    // Create device-local buffer (actual GPU resource)
    CreateBuffer(
        gpu,
        memorySize,
        usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // Enable transfer dst
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,                 //cpu can't see it
        buffer,
        memory
    );


    // Copy staging -> device
    CopyBuffer(gpu,commandManager, stagingBuffer, buffer, memorySize);

    // Cleanup
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingMemory, nullptr);
}

void BufferOps::CreateCPUVisibleBuffer(
    GPU gpu,
    CommandManager& commandManager,
    void* data,
    VkDeviceSize datasize,
    VkBufferUsageFlags usage,
    VkBuffer& buffer,
    VkDeviceMemory& memory){

    assert(datasize != 0);
    VkDeviceSize memorySize = datasize;
    auto device = gpu.GetVkDevice();

    auto paddedSize = BufferOps::AlignUp(memorySize, 256);   

    BufferOps::CreateBuffer(
        gpu,
        paddedSize,
        usage,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,  
        buffer,
        memory
    );

    // Upload data
    void* pGpuMemory;
    vkMapMemory(device, memory, 0, paddedSize, 0, &pGpuMemory);  //mapping gpu memory that the cpu can see
    memcpy(pGpuMemory, data, (size_t)memorySize);

    VkMappedMemoryRange flushRange = {};
    flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    flushRange.memory = memory;       // The memory to flush/invalidate
    flushRange.offset = 0;                  // Start from the beginning
    flushRange.size = paddedSize;    

    vkFlushMappedMemoryRanges(device,1, &flushRange); 
    vkUnmapMemory(device, memory);

}

VkDeviceSize BufferOps::AlignUp(VkDeviceSize value, VkDeviceSize alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void BufferResource::Preload(void* srcData, VkDeviceSize dataSize, uint32_t arraySize, uint32_t offset) {
    toDo = malloc(dataSize);
    memcpy(toDo, srcData, dataSize);
    this->dataSize = dataSize;
    this->arraySize = arraySize;
    this->offset = offset;
}

void BufferResource::UpdateHostVisibleData(GPU * gpu, void* srcData, VkDeviceSize dataSize, uint32_t arraySize, uint32_t offset ){

    assert(srcData != nullptr);
    assert(dataSize > 0);
    assert(memory != VK_NULL_HANDLE);
    
    // Map once per update
    void* mappedMemory = nullptr;
    vkMapMemory(gpu->GetVkDevice(), memory, 0, dataSize, 0, &mappedMemory);

    // Copy new data to mapped memory
    std::memcpy(mappedMemory, srcData, static_cast<size_t>(dataSize));

    // Flush the updated range (only needed if not host-coherent)
    VkMappedMemoryRange flushRange{};
    flushRange.sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    flushRange.memory = memory;
    flushRange.offset = 0;
    flushRange.size   = VK_WHOLE_SIZE;
    vkFlushMappedMemoryRanges(gpu->GetVkDevice(), 1, &flushRange);

    vkUnmapMemory(gpu->GetVkDevice(), memory);
}
