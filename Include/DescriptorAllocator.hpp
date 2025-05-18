#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "GPU.hpp"
#include "ResourceManager.hpp"
#include "BufferResource.hpp"

class DescriptorAllocator {
    public:
        DescriptorAllocator() = delete;
        DescriptorAllocator(std::shared_ptr<GPU> gpu);
        ~DescriptorAllocator();

        VkDescriptorPool GetPool()const {return descriptorPool;}
        std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts () {return descriptorSetLayouts;}
        std::vector<VkDescriptorSet>& GetDescriptorSets () {return descriptorSets;}

        VkDescriptorSet& GetDescriptorSet(uint32_t set){
            return descriptorSets[set];
        }

        VkDescriptorSetLayout& GetDescriptorSetLayout(uint32_t set){
            return descriptorSetLayouts[set];
        }

        void CreateDescriptorSetPool(std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet);
        void AllocateDescriptorSets();
        void CreateDescriptorSetLayout(const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet);
        void resetPool();

        //in our pipeline config, we allocate all the descriptor pools and sets we need
        //here you pass in the resource data of what you requested
        //if you requested data in more than 1 set, it should be reflect here.
        //the order MUST be correct
        //this is where we give the actual buffers, no longer a description. Vkbuffer has the actual data.
        void UpdateDescriptorSets(ResourceManager * manager){
        
            for (uint32_t frame = 0; frame < MAX_FRAMES; ++frame) {

                for (size_t setIndex = 0; setIndex < descriptorSetLayouts.size(); ++setIndex) {

                    const VkDescriptorSet& set = descriptorSets[frame * descriptorSetLayouts.size() + setIndex];
                    auto& resources = manager->GetDescriptorSet(setIndex);
        
                    std::vector<VkWriteDescriptorSet> writes;
                    std::vector<VkDescriptorBufferInfo> bufferInfos;
        
                    for (uint32_t binding = 0; binding < resources.size(); ++binding) {
                         
                        VkDescriptorBufferInfo bufferInfo{};
                        bufferInfo.buffer = resources[binding]->GetBuffer();
                        bufferInfo.offset = resources[binding]->GetOffSet();
                        bufferInfo.range  = resources[binding]->GetAlignedDataSize(256);
        
                        bufferInfos.push_back(bufferInfo);
        
                        VkWriteDescriptorSet write{};
                        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        write.dstSet = set;
                        write.dstBinding = binding;
                        write.dstArrayElement = 0;
                        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        write.descriptorCount = 1;
                        write.pBufferInfo = &bufferInfos.back();
        
                        writes.push_back(write);
                    }
        
                    vkUpdateDescriptorSets(gpu->GetVkDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
                }
            }
        }
    

        void Cleanup(); // explicit destruction


    protected:

        
 
        std::shared_ptr<GPU> gpu;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> descriptorSets;
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        std::vector<std::vector<std::string>> descriptorNamesPerSet;
        

    private:
        const uint32_t MAX_FRAMES = 3;

 
};