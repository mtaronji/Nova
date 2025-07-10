#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <cstring>

class CommandManager;
class GPU;

class BufferOps{

public:
 
    static void CreateBuffer(GPU& gpu, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    //copy from source buffer to destination buffer
    //if you have a staging buffer that is monolithic, you can use the soffset parameter for it's start
    //doffset is the destination buffer start offset
    static void CopyBuffer(GPU& gpu, CommandManager& commandManager, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize soffset, VkDeviceSize doffset);
    
    //vkBuffer
    static void EnsureDeviceBuffer(
            GPU&,
            CommandManager&,
            void* data,
            VkDeviceSize datasize,
            VkBufferUsageFlags usage,
            VkBuffer& buffer,
            VkDeviceMemory& memory,
            VkDeviceSize destinationBufferOffset,
            VkDeviceSize stagingBufferOffset);

    static void EnsureHostBuffer(
        GPU& gpu,
        CommandManager& commandManager,
        void* data,
        VkDeviceSize datasize,
        VkBufferUsageFlags usage,
        VkBuffer& buffer,
        VkDeviceMemory& memory,
        VkDeviceSize destBufferOffset);

    static void UpdateHostBuffer(
        GPU gpu,
        CommandManager& commandManager,
        void* data,
        size_t dataSize,
        VkBuffer& buffer,
        VkDeviceMemory& memory,
        VkDeviceSize destBufferOffset);

    static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    static VkDeviceSize AlignUp(VkDeviceSize value, VkDeviceSize alignment);

};



                                   