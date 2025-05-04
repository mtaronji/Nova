#pragma once


#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include "GPU.hpp"
#include "BufferOps.hpp"



class DescriptorAllocator {
    public:
        DescriptorAllocator() = delete;
        DescriptorAllocator(std::shared_ptr<GPU> gpu);
        ~DescriptorAllocator();

        VkDescriptorPool GetPool()const {return descriptorPool;}
        std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts () {return descriptorSetLayouts;}
        std::vector<VkDescriptorSet>& GetDescriptorSets () {return descriptorSets;}

        void CreateDescriptorSetPool(std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet);
        void AllocateDescriptorSets();
        void CreateDescriptorSetLayout(const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet);
        void resetPool();

        //in our pipeline config, we allocate all the descriptor pools and sets we need
        //here you pass in the resource data of what you requested
        //if you requested data in more than 1 set, it should be reflect here.
        //the order MUST be correct
        //this is where we give the actual buffers, no longer a description. Vkbuffer has the actual data.
        void UpdateDescriptorSets(std::vector<std::vector<VkBuffer>> buffersPerSet){

            
            for (uint32_t frame = 0; frame < MAX_FRAMES; ++frame) {

                for (size_t setIndex = 0; setIndex < descriptorSetLayouts.size(); ++setIndex) {

                    const VkDescriptorSet& set = descriptorSets[frame * descriptorSetLayouts.size() + setIndex];
                    const auto& buffers = buffersPerSet[setIndex];
        
                    std::vector<VkWriteDescriptorSet> writes;
                    std::vector<VkDescriptorBufferInfo> bufferInfos;
        
                    for (uint32_t binding = 0; binding < buffers.size(); ++binding) {
                        
        
                        VkDescriptorBufferInfo bufferInfo{};
                        // bufferInfo.buffer = novaBu;
                        // bufferInfo.offset = 0;
                        // bufferInfo.range  = novaBuf.size;
        
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
    

        void destroy(); // explicit destruction


    protected:

        
 
        std::shared_ptr<GPU> gpu;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> descriptorSets;
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        

    private:
        const uint32_t MAX_FRAMES = 3;

 
};