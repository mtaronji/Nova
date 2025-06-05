#include "BufferOps.hpp"
#include "CommandManager.hpp"
#include "GPU.hpp"

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

void BufferOps::CopyBuffer(GPU gpu, CommandManager& commandManager, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize stagingBufferOffset, VkDeviceSize destinationBufferOffset) {

    VkCommandBuffer commandBuffer = commandManager.BeginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    copyRegion.dstOffset = destinationBufferOffset;
    copyRegion.srcOffset = stagingBufferOffset;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    commandManager.EndSingleTimeCommands(commandBuffer);
}

void BufferOps::EnsureDeviceBuffer(
    GPU gpu,
    CommandManager& commandManager,
    void* data,
    size_t datasize,
    VkBufferUsageFlags usage,
    VkBuffer& buffer,
    VkDeviceMemory& memory,
    VkDeviceSize destinationBufferOffset,
    VkDeviceSize stagingBufferOffset){

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
    vkMapMemory(device, stagingMemory, 0, paddedSize, 0, &pGpuMemory);  //mapping cpu memory that the gpu can see
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
    if(buffer == VK_NULL_HANDLE){
        CreateBuffer(
        gpu,
        paddedSize,
        usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // Enable transfer dst
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,                 //cpu can't see it
        buffer,
        memory);
    }


    // Copy staging -> device
    CopyBuffer(gpu,commandManager, stagingBuffer, buffer, paddedSize, stagingBufferOffset, destinationBufferOffset);

    // Cleanup
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingMemory, nullptr);
}

void BufferOps::EnsureHostBuffer(
    GPU gpu,
    CommandManager& commandManager,
    void* data,
    size_t datasize,
    VkBufferUsageFlags usage,
    VkBuffer& buffer,
    VkDeviceMemory& memory,
    VkDeviceSize destinationBufferOffset){

    assert(datasize != 0);
    VkDeviceSize memorySize = datasize;
    auto device = gpu.GetVkDevice();

    auto paddedSize = BufferOps::AlignUp(memorySize, 256);   
    
    if(buffer == VK_NULL_HANDLE){
        BufferOps::CreateBuffer(
        gpu,
        paddedSize,
        usage,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,  
        buffer,
        memory);
    }
        
    // Upload data
    void* pGPUVisibleSystemMemory;
    vkMapMemory(device, memory, destinationBufferOffset, paddedSize, 0, &pGPUVisibleSystemMemory);  //mapping cpu memory that the gpu can see
    memcpy(pGPUVisibleSystemMemory, data, (size_t)memorySize);

    VkMappedMemoryRange flushRange = {};
    flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    flushRange.memory = memory;       // The memory to flush/invalidate
    flushRange.offset = destinationBufferOffset;                  // Start from the beginning
    flushRange.size = paddedSize;    

    vkFlushMappedMemoryRanges(device,1, &flushRange); 
    vkUnmapMemory(device, memory);
}

void BufferOps::UpdateHostBuffer(
        GPU gpu,
        CommandManager& commandManager,
        void* data,
        size_t dataSize,
        VkBuffer& buffer,
        VkDeviceMemory& memory,
        VkDeviceSize destinationBufferOffset){
        
    // Upload data
    
    auto device = gpu.GetVkDevice();
    void* pGPUVisibleSystemMemory;
    auto memorySize = BufferOps::AlignUp(dataSize, 256);
    vkMapMemory(device, memory, destinationBufferOffset, memorySize, 0, &pGPUVisibleSystemMemory);  //mapping cpu memory that the gpu can see
    memcpy(pGPUVisibleSystemMemory, data, (size_t)dataSize);

    VkMappedMemoryRange flushRange = {};
    flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    flushRange.memory = memory;                             // The memory to flush/invalidate
    flushRange.offset = destinationBufferOffset;                  // Start from the beginning
    flushRange.size = memorySize;    

    vkFlushMappedMemoryRanges(device,1, &flushRange); 
    vkUnmapMemory(device, memory);
}
VkDeviceSize BufferOps::AlignUp(VkDeviceSize value, VkDeviceSize alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

uint32_t BufferOps::FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}
