#include <vulkan/vulkan.h>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <cstring>
#include "GPU.hpp"
#include "CommandManager.hpp"

void BufferOps::CreateBuffer(GPU& gpu, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {

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
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void BufferOps::CopyBuffer(GPU& gpu, CommandManager& commandManager, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

    VkCommandBuffer commandBuffer = commandManager.BeginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    commandManager.EndSingleTimeCommands(commandBuffer);
}

void BufferOps::CreateDataBuffer(
    GPU& gpu,
    CommandManager& commandManager,
    void* data,
    uint64_t datasize,
    VkBufferUsageFlags usage,
    VkBuffer& buffer,
    VkDeviceMemory& memory){

    assert(datasize != 0);
    VkDeviceSize bufferSize = datasize;
    auto device = gpu.GetVkDevice();

    // Create staging buffer (host-visible)
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    CreateBuffer(
        gpu,
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,  
        stagingBuffer,
        stagingMemory
    );

    VkMappedMemoryRange flushRange = {};
    flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    flushRange.memory = stagingMemory;       // The memory to flush/invalidate
    flushRange.offset = 0;                  // Start from the beginning
    flushRange.size = VK_WHOLE_SIZE;       

    // Upload data
    void* pGpuMemory;
    vkMapMemory(device, stagingMemory, 0, bufferSize, 0, &pGpuMemory);  //mapping gpu memory that the cpu can see
    memcpy(pGpuMemory, data, (size_t)bufferSize);
    vkFlushMappedMemoryRanges(device,1, &flushRange); 
    vkUnmapMemory(device, stagingMemory);

    // Create device-local buffer (actual GPU resource)
    CreateBuffer(
        gpu,
        bufferSize,
        usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // Enable transfer dst
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,                 //cpu can't see it
        buffer,
        memory
    );


    // Copy staging -> device
    CopyBuffer(gpu,commandManager, stagingBuffer, buffer, bufferSize);

    // Cleanup
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingMemory, nullptr);
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