#pragma once

#include "NovaConstants.hpp"
#include "Resource.hpp"
#include <vulkan/vulkan.hpp>

class GPU;

//this is for resources that go in buffers
class BufferResource : public Resource {
public:
    BufferResource(const BufferResource&) = delete;
    BufferResource& operator=(const BufferResource&) = delete;
    BufferResource(BufferResource&& other) noexcept :Resource(std::move(other)) {

        this->usage = other.usage;
        this->offset = other.offset;
    }
    BufferResource& operator=(BufferResource&& other) noexcept {

        if (this != &other) {
            Resource::operator=(std::move(other)); // 
            usage = other.usage;
            offset = other.offset;

            other.usage = 0;
            other.offset = 0;
        }
        return *this;
    }

    ~BufferResource() override = default;

    static BufferResource Create(VkBufferUsageFlags usage, uint32_t copies, uint32_t set = NOT_SET, uint32_t binding = NOT_SET) {
        return BufferResource(usage, copies, set, binding);
    }

    void SetBuffer(VkBuffer buffer) { this->buffer = buffer; }

    void SetMemory(VkDeviceMemory memory) { this->memory = memory; }
    VkDeviceMemory& GetMemory() { return memory; }

    VkBufferUsageFlags GetUsage() const { return usage; }
    VkDescriptorType GetDescriptorType() const { return descriptorType; }

    size_t GetDataSize() const { return dataSize; }
    size_t GetPreviousDataSize() const { return previousSize; }
    size_t GetAlignedDataSize(VkDeviceSize alignment);
    void SetOffSet(VkDeviceSize offset) { this->offset = offset; }
    VkDeviceSize GetOffSet() const { return offset; }
    uint32_t GetArraySize() const;
    
    void Upload(void* srcData, VkDeviceSize dataSize, uint32_t arraySize = NOT_SET);
    void Cleanup(GPU* gpu) override;

protected:
  
private:
    BufferResource(VkBufferUsageFlags usage, uint32_t copies, uint32_t set, uint32_t binding);

    VkBufferUsageFlags usage{};
    VkDeviceSize offset = 0;
};