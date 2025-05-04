#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <cassert>
#include <cstring>
// You first create the VkBuffer (saying "I need a buffer 512 bytes long for vertex data").

// Then you get the memory requirements and allocate a block of VkDeviceMemory.

// Finally, you bind the memory to the buffer using:

// vkBindBufferMemory(device, buffer, bufferMemory, 0);

// From that point on, the buffer is “powered” by the actual memory — just like plugging a power cord into a device.

//allocate memory to buffers
class CommandManager;
class GPU;

struct BufferOps{

 
    static void CreateBuffer(GPU& gpu, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);


    static void CopyBuffer(GPU& gpu, CommandManager& commandManager, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    static void CreateDataBuffer(
            GPU& gpu,
            CommandManager& commandManager,
            void* data,
            uint64_t datasize,
            VkBufferUsageFlags usage,
            VkBuffer& buffer,
            VkDeviceMemory& memory);

   
    static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
};

struct BufferResource{
    
    BufferResource(VkBufferUsageFlags usage):usage(usage){
        assert(usage != VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        assert(usage != VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);	
        assert(usage != VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT);	
        assert(usage != VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT);

    }
    BufferResource(uint32_t set, uint32_t binding, VkBufferUsageFlags usage)
        :binding(binding), set(set), buffer(VK_NULL_HANDLE),memory(VK_NULL_HANDLE), size(0),usage(usage){

    }
    VkBuffer& GetBuffer() {return buffer;}
    VkBufferUsageFlags GetUsage() const {return usage;}
    

    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDeviceSize size;
    size_t dataSize;
    uint32_t offset;
    uint32_t arraySize;

    std::string name; //camera, lighting, etc
    bool needsUpdate = true;
    void* mappedData = nullptr;
    void UpdateData(void* srcData, size_t dataSize, uint32_t arraySize, uint32_t offset = 0) {
        mappedData = malloc(dataSize);
        memcpy(mappedData, srcData,dataSize);
        this->dataSize = dataSize;
        this->arraySize = arraySize;
        this->offset = offset;
    }

    VkDeviceMemory& GetMemory() {return memory;}


    private:
        uint32_t binding;
        uint32_t set;
        VkBufferUsageFlags usage;
};
