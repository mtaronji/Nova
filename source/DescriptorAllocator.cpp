#include "DescriptorAllocator.hpp"

DescriptorAllocator::DescriptorAllocator(std::shared_ptr<GPU> gpu)
    : gpu(gpu) {}

    DescriptorAllocator::~DescriptorAllocator() {
    destroy();
}

//descriptor pools create containers for the ubo objects. They don't actually manage the memory or anything
//If I have a descriptor set for a camera, and I want to have a ubo for each frame, and i want 3 frames(triple buffering)
//then I will need to specify a pool of { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 } 
//I will create one ubo for the camera all ahead of time that you will bind into
void DescriptorAllocator::CreatePool(uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes) {

    VkDescriptorPoolCreateInfo poolInfo{}; 
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxSets;
    poolInfo.flags = 0; // Optional: you could add VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT

    if (vkCreateDescriptorPool(gpu->GetVkDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}


VkDescriptorSet DescriptorAllocator::AllocateDescriptorSet(VkDescriptorSetLayout layout) {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkDescriptorSet descriptorSet;
    if (vkAllocateDescriptorSets(gpu->GetVkDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor set!");
    }

    return descriptorSet;
}

void DescriptorAllocator::resetPool() {
    if (descriptorPool != VK_NULL_HANDLE) {
        vkResetDescriptorPool(gpu->GetVkDevice(), descriptorPool, 0);
    }
}

void DescriptorAllocator::destroy() {
    if (descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(gpu->GetVkDevice(), descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }
}