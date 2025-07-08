#pragma once

#include <vulkan/vulkan.hpp>
#include "NovaConstants.hpp"
class GPU;

/// <summary>
/// base resource class for all resources on the GPU
/// </summary>
/// 
/// 



class Resource {
public:

    Resource(uint32_t copies, uint32_t set, uint32_t binding)
        : copies(copies), set(set), binding(binding) {
       
    }
    Resource(Resource&& other) noexcept
        : copies(other.copies),
        set(other.set),
        binding(other.binding),
        memory(other.memory),
        buffer(other.buffer),
        image(other.image),
        kind(other.kind),
        needsUpdate(other.needsUpdate),
        descriptorType(other.descriptorType),
        data(other.data)
    {
        other.memory = VK_NULL_HANDLE;
        other.buffer = VK_NULL_HANDLE;
        other.image = VK_NULL_HANDLE;

        if (other.data) {
            free(other.data);
            other.data = nullptr;
        }
    }

    Resource& operator=(Resource&& other) noexcept {

        if (this != &other) {
            copies = other.copies;
            set = other.set;
            binding = other.binding;
            memory = other.memory;
            needsUpdate = other.needsUpdate;
            buffer = other.buffer;
            image = other.image;
            kind = other.kind;
            data = other.data;
            descriptorType = other.descriptorType;

            other.memory = VK_NULL_HANDLE;
            other.buffer = VK_NULL_HANDLE;
            other.image = VK_NULL_HANDLE;
            if (other.data) {
                free(other.data);
                other.data = nullptr;
            }         
        }
        return *this;
    }

    virtual ~Resource() = default;

    uint32_t GetBinding() const { return binding; }
    uint32_t GetSet() const { return set; }

    uint32_t GetCopyCount() const { return copies; }
    bool NeedsUpdate() const { return needsUpdate; }
    void* GetData() const { return data; }
    void Upload(void* srcData, VkDeviceSize dataSize, uint32_t arraySize = NOT_SET);
    virtual VkBuffer& GetBuffer() { 
        assert(kind == ResourceKind::Buffer && "GetBuffer() called on non-buffer resource");
        return buffer; 
    }
    virtual VkImage& GetImage() { 
        assert(kind == ResourceKind::Image && "GetBuffer() called on non-buffer resource");
        return image; 
    }

    virtual void Cleanup(GPU* gpu) = 0;

protected:
    VkDescriptorType descriptorType{};
    uint32_t copies;
    uint32_t set;
    uint32_t binding;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    bool needsUpdate = true;
    void* data = nullptr;
    enum class ResourceKind {
        Buffer,
        Image,
        None,
    } kind = ResourceKind::None;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkImage image = VK_NULL_HANDLE;

    size_t dataSize = 0;
    size_t previousSize = 0;
    uint32_t arraySize = NOT_SET;  //for indice data

private:
 
};