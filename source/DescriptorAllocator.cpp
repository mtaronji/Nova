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

void DescriptorAllocator::CreateDescriptorSetPool(std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet, uint32_t MAX_FRAMES) {

    std::unordered_map<VkDescriptorType, uint32_t> descriptorCounts;
    // Iterate through all descriptor set layouts and count descriptors by type
    for (const auto& setBindings : descriptorBindingsPerSet) {
        for (const auto& binding : setBindings) {
            // Increment the count for this descriptor type
            descriptorCounts[binding.descriptorType] +=  MAX_FRAMES * binding.descriptorCount;
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
    poolInfo.maxSets = static_cast<uint32_t> (descriptorBindingsPerSet.size() * MAX_FRAMES);
    poolInfo.flags = 0; // Optional: you could add VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT

    if (vkCreateDescriptorPool(gpu->GetVkDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

void DescriptorAllocator::CreateDescriptorSetLayout(std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet, std::vector<VkDescriptorSetLayout>& descriptorSetLayoutOut){

    descriptorSetLayoutOut.resize(descriptorBindingsPerSet.size());  
    
    for(int i = 0; i < descriptorBindingsPerSet.size(); i++){ 
        descriptorSetLayoutOut[i] = {};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t> (descriptorBindingsPerSet[i].size());
        layoutInfo.pBindings = descriptorBindingsPerSet[i].data();
        if (vkCreateDescriptorSetLayout(gpu->GetVkDevice(), &layoutInfo, nullptr, &descriptorSetLayoutOut[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }         
}


void DescriptorAllocator::AllocateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkDescriptorSetLayout>& descriptorSetLayouts) {
      
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    allocInfo.pSetLayouts = descriptorSetLayouts.data();
    
    if (vkAllocateDescriptorSets(gpu->GetVkDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }
}

void DescriptorAllocator::AllocateDescriptorSet(VkDescriptorSet& descriptorSet, VkDescriptorSetLayout& descriptorSetLayout) {
      
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(1);
    allocInfo.pSetLayouts = &descriptorSetLayout;
    
    if (vkAllocateDescriptorSets(gpu->GetVkDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }
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

// void DescriptorAllocator::UpdateDescriptorSet( std::vector<BufferResource*> descriptorBindingBufferResources,
//                                                VkDescriptorSet descriptorSet,
//                                                uint32_t setIndex,
//                                                VkDescriptorSetLayout descriptorSetLayout,
//                                                std::vector<VkDescriptorSetLayoutBinding>& descriptorBindings,
//                                                uint32_t copies
//                                                ){
    

//     for(size_t copy = 0; copy < copies; copy++){
    
//         std::vector<VkWriteDescriptorSet> writes;
//         std::vector<VkDescriptorBufferInfo> bufferInfos;

//         for (uint32_t binding = 0; binding < descriptorBindings.size(); ++binding) {
            
//             VkDescriptorBufferInfo bufferInfo{};
//             auto initialOffset = descriptorBindingBufferResources[binding]->GetOffSet();
//             auto offset = initialOffset * descriptorBindingBufferResources[binding]->GetAlignedDataSize(256) * copy;
//             bufferInfo.buffer = descriptorBindingBufferResources[binding]->GetBuffer();
//             bufferInfo.offset = offset;
//             bufferInfo.range  = descriptorBindingBufferResources[binding]->GetAlignedDataSize(256);

//             bufferInfos.push_back(bufferInfo);

//             VkWriteDescriptorSet write{};
//             write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//             write.dstSet = descriptorSet;
//             write.dstBinding = binding;
//             write.dstArrayElement = 0;
//             write.descriptorType = descriptorBindings[binding].descriptorType;
//             write.descriptorCount = 1;
//             write.pBufferInfo = &bufferInfos.back();

//             writes.push_back(write);
//         }

//         vkUpdateDescriptorSets(gpu->GetVkDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
//     }
    
// }