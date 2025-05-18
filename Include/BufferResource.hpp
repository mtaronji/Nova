#pragma once

#include <vulkan/vulkan.hpp>
#include "BufferOps.hpp"
#include "GPU.hpp"

constexpr uint32_t NOT_SET = 0xffffffff;
struct BufferResource{

    BufferResource(const BufferResource&) = delete;
    BufferResource& operator=(const BufferResource&) = delete;

    ~BufferResource(){
       
    }

    VkBuffer& GetBuffer() {return buffer;}
    void SetBuffer(VkBuffer buffer){ this->buffer = buffer;}
    void SetMemory(VkDeviceMemory memory){this->memory = memory;}
    void SetOffSet(uint32_t offset){this->offset = offset;}
    uint32_t GetOffSet(){return this->offset;} 
    VkBufferUsageFlags GetUsage() const {return usage;}
    VkDescriptorType GetDescriptorType() const {return descriptorType;}
    VkDeviceMemory& GetMemory() {return memory;}
    size_t GetDataSize() const {return dataSize;}
    size_t GetPreviousDataSize() { return previousSize;}
    size_t GetAlignedDataSize(VkDeviceSize alignment){
        return BufferOps::AlignUp(dataSize, alignment);
    }
    uint32_t GetBinding() const {return binding;}
    uint32_t GetSet() const {return set;}
    uint32_t GetArraySize() const {return arraySize;}
    void* GetData() const {return data;}

 
    bool needsUpdate = true;
    void* data = nullptr; //points to the actual buffer data
    void Upload(void* srcData, VkDeviceSize dataSize, uint32_t arraySize = NOT_SET); 

    void Cleanup(GPU* gpu){
        if(buffer){
            vkDestroyBuffer(gpu->GetVkDevice(), buffer, nullptr);
            buffer = VK_NULL_HANDLE;
        }
        if(memory){
            vkFreeMemory(gpu->GetVkDevice(), memory, nullptr);
            memory = VK_NULL_HANDLE;
        }

        if(data != nullptr){
            free(data);
            data = nullptr;
        }      
    }

    static BufferResource* Create(VkBufferUsageFlags usage, uint32_t set = NOT_SET, uint32_t binding = NOT_SET){
        return new BufferResource(usage, set, binding);
    }

    protected:
        BufferResource(VkBufferUsageFlags usage, uint32_t set = NOT_SET, uint32_t binding = NOT_SET);

    private:
        uint32_t set;
        uint32_t binding;
        VkBufferUsageFlags usage{};
        VkDescriptorType descriptorType{};
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        size_t dataSize = 0;
        uint32_t offset = static_cast<uint32_t> (0);
        uint32_t arraySize;
        size_t previousSize = 0;
};