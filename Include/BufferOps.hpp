#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <cassert>
#include <cstring>
#include "GPU.hpp"
// You first create the VkBuffer (saying "I need a buffer 512 bytes long for vertex data").

// Then you get the memory requirements and allocate a block of VkDeviceMemory.

// Finally, you bind the memory to the buffer using:

// vkBindBufferMemory(device, buffer, bufferMemory, 0);

// From that point on, the buffer is “powered” by the actual memory — just like plugging a power cord into a device.

//allocate memory to buffers
class CommandManager;
class GPU;

struct BufferOps{

 
    static void CreateBuffer(GPU gpu, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);


    static void CopyBuffer(GPU gpu, CommandManager& commandManager, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    static void CreateOnGPUBuffer(
            GPU gpu,
            CommandManager& commandManager,
            void* data,
            VkDeviceSize datasize,
            VkBufferUsageFlags usage,
            VkBuffer& buffer,
            VkDeviceMemory& memory);

    static void CreateCPUVisibleBuffer(
        GPU gpu,
        CommandManager& commandManager,
        void* data,
        VkDeviceSize datasize,
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

    static VkDeviceSize AlignUp(VkDeviceSize value, VkDeviceSize alignment);

};

struct BufferResource{

    BufferResource(const BufferResource&) = delete;
    BufferResource& operator=(const BufferResource&) = delete;

    ~BufferResource(){
       
    }

    void Cleanup(GPU* gpu){
        if(buffer){
            vkDestroyBuffer(gpu->GetVkDevice(), buffer, nullptr);
            buffer = VK_NULL_HANDLE;
        }
        if(memory){
            vkFreeMemory(gpu->GetVkDevice(), memory, nullptr);
            memory = VK_NULL_HANDLE;
        }

        if(toDo != nullptr){
            free(toDo);
            toDo = nullptr;
        }
        
    }

    BufferResource(VkBufferUsageFlags usage):usage(usage){
        assert(usage != VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        assert(usage != VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);	
        assert(usage != VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT);	
        assert(usage != VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT);

    }
    BufferResource(uint32_t set, uint32_t binding, VkBufferUsageFlags usage)
        :binding(binding), set(set), buffer(VK_NULL_HANDLE),memory(VK_NULL_HANDLE), dataSize(0),usage(usage){
        if (usage == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT){descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;}
        if(usage == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT){descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;}
        if(usage == VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT){descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;}
        if(usage == VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT){descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;}
    }

    VkBuffer& GetBuffer() {return buffer;}
    VkBufferUsageFlags GetUsage() const {return usage;}
    VkDescriptorType GetDescriptorType() const {return descriptorType;}
    VkDeviceMemory& GetMemory() {return memory;}
    size_t GetDataSize() const {return dataSize;}
    uint32_t GetBinding() const {return binding;}
    uint32_t GetSet() const {return set;}
    uint32_t GetArraySize() const {return arraySize;}
    void* GetInitialDataP() const {return toDo;}

 
    bool needsUpdate = true;
    void* toDo = nullptr; //points to data for various buffer resources but can't initialize because we don't have vulkan init yet
    void Preload(void* srcData, VkDeviceSize dataSize, uint32_t arraySize, uint32_t offset = 0); 
    void UpdateHostVisibleData(GPU* gpu, void* srcData, VkDeviceSize dataSize, uint32_t arraySize, uint32_t offset = 0);
    void FreeInitialDataMemory(){ if(toDo != nullptr) {free(toDo); toDo = nullptr;}}

    private:
        uint32_t binding;
        uint32_t set;
        VkBufferUsageFlags usage{};
        VkDescriptorType descriptorType{};
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkDeviceSize dataSize;
        uint32_t offset;
        uint32_t arraySize;
};
