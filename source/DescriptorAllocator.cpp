#include "DescriptorAllocator.hpp"

DescriptorAllocator::DescriptorAllocator(std::shared_ptr<GPU> gpu)
    : gpu(gpu) {
        descriptorSets.resize(MAX_FRAMES);
    }

    DescriptorAllocator::~DescriptorAllocator() {
    destroy();
}

//descriptor pools create containers for the ubo objects. They don't actually manage the memory or anything
//If I have a descriptor set for a camera, and I want to have a ubo for each frame, and i want 3 frames(triple buffering)
//then I will need to specify a pool of { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 } 
//I will create one ubo for the camera all ahead of time that you will bind into
void DescriptorAllocator::CreateDescriptorSetPool(uint32_t maxSets, const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet) {

    std::unordered_map<VkDescriptorType, uint32_t> descriptorCounts;

    // Iterate through all descriptor set layouts and count descriptors by type
    for (const auto& setBindings : descriptorBindingsPerSet) {
        for (const auto& binding : setBindings) {
            // Increment the count for this descriptor type
            descriptorCounts[binding.descriptorType] += binding.descriptorCount * binding.descriptorCount;
        }
    }

    std::vector<VkDescriptorPoolSize> poolSizes;
    // Fill the poolSizes vector with the results from the descriptorCounts map
    poolSizes.clear();  // Clear any previous data
    poolSizes.reserve(descriptorCounts.size());  // Reserve space to avoid reallocation

    for (const auto& [type, count] : descriptorCounts) {
        poolSizes.push_back({.type = type, .descriptorCount = count});
    }


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

void DescriptorAllocator::CreateDescriptorSetLayout(const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet){

    descriptorSetLayouts.resize(descriptorSets.size());
    for(int i = 0; i < descriptorBindingsPerSet.size(); i++){
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t> (descriptorBindingsPerSet[i].size());
        layoutInfo.pBindings = descriptorBindingsPerSet[i].data();
        if (vkCreateDescriptorSetLayout(gpu->GetVkDevice(), &layoutInfo, nullptr, &descriptorSetLayouts[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }         
}

void DescriptorAllocator::AllocateDescriptorSets() {

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    allocInfo.pSetLayouts = descriptorSetLayouts.data();

    if (vkAllocateDescriptorSets(gpu->GetVkDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor set!");
    }
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