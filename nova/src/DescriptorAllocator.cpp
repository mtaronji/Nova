#include "DescriptorAllocator.hpp"

DescriptorAllocator::DescriptorAllocator(std::shared_ptr<GPU> gpu)
    : gpu(gpu) {

    }

DescriptorAllocator::~DescriptorAllocator() {
    
}

//descriptor pools create containers for the ubo objects. They don't actually manage the memory or anything
//If I have a descriptor set for a camera, and I want to have a ubo for each frame, and i want 3 frames(triple buffering)
//then I will need to specify a pool of { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 } 
//I will create one ubo for the camera all ahead of time that you will bind into
//this pool will take in all bindingsperset for each graphics pipeline and create a pool large enough to allocate one per object per frame

void DescriptorAllocator::CreateDescriptorSetPool(std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets, VkDescriptorPoolCreateFlags flags) {
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxSets;
    poolInfo.flags = flags;

    auto result = vkCreateDescriptorPool(gpu->GetVkDevice(), &poolInfo, nullptr, &descriptorPool);
    assert(result == VK_SUCCESS && "Failed to create descriptor pool!");
}

void DescriptorAllocator::CreateDescriptorSetLayout(std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet, std::vector<VkDescriptorSetLayout>& descriptorSetLayoutOut){

    descriptorSetLayoutOut.resize(descriptorBindingsPerSet.size());  
    
    for(int i = 0; i < descriptorBindingsPerSet.size(); i++){ 
        descriptorSetLayoutOut[i] = {};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t> (descriptorBindingsPerSet[i].size());
        layoutInfo.pBindings = descriptorBindingsPerSet[i].data();
        auto result = vkCreateDescriptorSetLayout(gpu->GetVkDevice(), &layoutInfo, nullptr, &descriptorSetLayoutOut[i]);
        assert(result == VK_SUCCESS &&"failed to create descriptor set layout!");
        
    }         
}


void DescriptorAllocator::AllocateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkDescriptorSetLayout>& descriptorSetLayouts) {
      
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    allocInfo.pSetLayouts = descriptorSetLayouts.data();
    
    auto result = vkAllocateDescriptorSets(gpu->GetVkDevice(), &allocInfo, descriptorSets.data());
    assert(result == VK_SUCCESS && "Failed to allocate descriptor sets!");
    
}

void DescriptorAllocator::AllocateDescriptorSet(VkDescriptorSet& descriptorSet, VkDescriptorSetLayout& descriptorSetLayout) {
      
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(1);
    allocInfo.pSetLayouts = &descriptorSetLayout;
    
    auto result = vkAllocateDescriptorSets(gpu->GetVkDevice(), &allocInfo, &descriptorSet);
    assert(result == VK_SUCCESS && "Failed to allocate descriptor sets!");
   
}

void DescriptorAllocator::resetPool() {
    if (descriptorPool != VK_NULL_HANDLE) {
        vkResetDescriptorPool(gpu->GetVkDevice(), descriptorPool, 0);
    }
}

void DescriptorAllocator::Cleanup() {
    if (descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(gpu->GetVkDevice(), descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }   
}
