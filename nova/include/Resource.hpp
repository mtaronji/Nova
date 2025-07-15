#pragma once

#include <vulkan/vulkan.hpp>
#include "NovaConstants.hpp"
#include "BufferOps.hpp"
class GPU;

/// <summary>
/// base resource class for all resources on the GPU
/// </summary>
/// 
/// 
enum class ResourceType{
    IMAGE,
    BUFFER,
    NOT_SET
};

class Resource {
public:

    Resource(uint32_t copies, VkMemoryPropertyFlags memoryProperties)
        : Copies(copies), MemoryProperties(memoryProperties){
            
       auto requestsHostVisibleMemory = (memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
       auto requestedDeviceMemory = (memoryProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
       assert(requestsHostVisibleMemory != requestedDeviceMemory && "Can only specify host visible memory or device memory");

    }
    Resource(Resource&& other) noexcept
        : Copies(other.Copies),
        Memory(other.Memory),
        Data(other.Data),
        DataSize(other.DataSize),
        MemoryProperties(other.MemoryProperties)
    {
        other.Memory = VK_NULL_HANDLE;
        other.Data = nullptr;
        other.Copies = 0;
        other.DataSize = 0;
    }

    Resource& operator=(Resource&& other) noexcept {

        if (this != &other) {
            Copies = other.Copies;
            Memory = other.Memory;
            Data = other.Data;
            DataSize = other.DataSize;
            MemoryProperties = other.MemoryProperties;
            other.Memory = VK_NULL_HANDLE;          
        }
        return *this;
    }

    size_t GetAlignedDataSize(VkDeviceSize alignment){
        return BufferOps::AlignUp(DataSize, alignment);
    }
    virtual ~Resource() = default;

    virtual void Cleanup(GPU* gpu) = 0;

    uint32_t Copies;
    VkDeviceMemory Memory = VK_NULL_HANDLE;
    void* Data = nullptr;
    size_t DataSize = 0;
    ResourceType resourceType = ResourceType::NOT_SET;
    VkMemoryPropertyFlags MemoryProperties;

protected:
   
private:
 
};