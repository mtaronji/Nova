#pragma once
#include <vulkan/vulkan.h>
#include <memory>
#include <variant>
#include "BufferResource.hpp"
#include "ImageResource.hpp"

using ResourceVariant = std::variant<std::shared_ptr<BufferResource>, std::shared_ptr<ImageResource>>;

//Base descriptor
//This describes any resource that will be used as a descriptor set
class Descriptor {
public:
    Descriptor(uint32_t set, uint32_t binding, uint32_t descriptorCount, VkDescriptorType descriptorType )
        :Set(set), Binding(binding), DescriptorType(descriptorType), DescriptorCount(descriptorCount){}
    

    virtual ~Descriptor() = default;
    
    virtual ResourceVariant GetResource() = 0;

    uint32_t Set;
    uint32_t Binding;
    uint32_t DescriptorCount;
    VkDescriptorType DescriptorType;


protected:

};

//this is descriptor sets that use vkbuffers for their underlying memory.
//These are usually contigous resources like vertices indices, but also uniform buffers
class BufferDescriptor : public Descriptor{
public:
    BufferDescriptor(uint32_t set, uint32_t binding, uint32_t descriptorCount, VkDescriptorType descriptorType, std::shared_ptr<BufferResource> resource )
        :Descriptor(set, binding, descriptorCount, descriptorType), resource(resource) {
            
        }

    ResourceVariant GetResource() override {return this->resource;}

    std::shared_ptr<BufferResource> resource;
protected:
    
};


//this type of descriptor set is for image type resources. 
//Think data that is 2 or 3 dimensional. 
//each image descriptor needs a 
class ImageDescriptor : public Descriptor{
public:
    ImageDescriptor(uint32_t set, uint32_t binding, uint32_t descriptorCount, VkDescriptorType descriptorType,uint32_t layers, uint32_t mipLevels, std::shared_ptr<ImageResource> resource)
        :Descriptor(set, binding, descriptorCount, descriptorType), resource(resource), key{ .mipLevels = mipLevels, .layers = layers } {
    }

    ResourceVariant GetResource() override{return this->resource;}
    std::shared_ptr<ImageResource> resource;
    MipLayer key; //each descriptor for a texture should have it's layer count and mip level. We will then create this new texture if it doesn't exist or return it
protected:
    
 
};