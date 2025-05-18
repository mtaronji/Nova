#pragma once

#include <vulkan/vulkan.h>



class CommandManager;
class GPU;

struct BufferOps{

 
    static void CreateBuffer(GPU gpu, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    static void CopyBuffer(GPU gpu, CommandManager& commandManager, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, uint32_t soffset, uint32_t doffset);
    
    //vkBuffer
    static void EnsureDeviceBuffer(
            GPU gpu,
            CommandManager& commandManager,
            void* data,
            VkDeviceSize datasize,
            VkBufferUsageFlags usage,
            VkBuffer& buffer,
            VkDeviceMemory& memory,
            uint32_t destBufferOffset,
            uint32_t stagingBufferOffset);

    static void EnsureHostBuffer(
        GPU gpu,
        CommandManager& commandManager,
        void* data,
        VkDeviceSize datasize,
        VkBufferUsageFlags usage,
        VkBuffer& buffer,
        VkDeviceMemory& memory,
        uint32_t destBufferOffset);

    static void UpdateHostBuffer(
        GPU gpu,
        CommandManager& commandManager,
        void* data,
        size_t dataSize,
        VkBuffer& buffer,
        VkDeviceMemory& memory,
        uint32_t destBufferOffset);

    static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    static VkDeviceSize AlignUp(VkDeviceSize value, VkDeviceSize alignment);

};



                                   