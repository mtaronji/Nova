#pragma once

#include "NovaConstants.hpp"

#include <vulkan/vulkan.hpp>
#include <cstring>

class GPU;
struct BufferResource{

    BufferResource(const BufferResource&) = delete;
    BufferResource& operator=(const BufferResource&) = delete;

    ~BufferResource(){
       
    }

    VkBuffer& GetBuffer() {return buffer;}
    void SetBuffer(VkBuffer buffer){ this->buffer = buffer;}
    void SetMemory(VkDeviceMemory memory){this->memory = memory;}
    void SetOffSet(VkDeviceSize offset){this->offset= offset;}
    VkDeviceSize GetOffSet(){return offset;} 
    VkBufferUsageFlags GetUsage() const {return usage;}
    VkDescriptorType GetDescriptorType() const {return descriptorType;}
    VkDeviceMemory& GetMemory() {return memory;}
    size_t GetDataSize() const {return dataSize;}
    size_t GetPreviousDataSize() { return previousSize;}
    size_t GetAlignedDataSize(VkDeviceSize alignment);
    uint32_t GetBinding() const {return binding;}
    uint32_t GetSet() const {return set;}
    uint32_t GetArraySize() const {return arraySize;}
    void* GetData() const {return data;}
    uint32_t GetCopyCount() {return copies;}
 
    bool needsUpdate = true;
    void* data = nullptr; //points to the actual buffer data
    void Upload(void* srcData, VkDeviceSize dataSize, uint32_t arraySize = NOT_SET); 

    void Cleanup(GPU* gpu);

    static BufferResource* Create(VkBufferUsageFlags usage, uint32_t copies, uint32_t set = NOT_SET, uint32_t binding = NOT_SET){
        return new BufferResource(usage, copies, set, binding);
    }

    protected:
        BufferResource(VkBufferUsageFlags usage, uint32_t copies, uint32_t set, uint32_t binding);

    private:
        uint32_t set;
        uint32_t binding;
        uint32_t copies; //if you want per frame resources, this 
        VkBufferUsageFlags usage{};
        VkDescriptorType descriptorType{};
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        size_t dataSize = 0;
        VkDeviceSize offset;
        uint32_t arraySize;
        size_t previousSize = 0;
};