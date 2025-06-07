#pragma once

#include "GPU.hpp"
#include "ResourceManager.hpp"
#include "BufferResource.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stdexcept>

class DescriptorAllocator {
    public:
        DescriptorAllocator() = delete;
        DescriptorAllocator(std::shared_ptr<GPU> gpu);
        ~DescriptorAllocator();

        VkDescriptorPool GetPool()const {return descriptorPool;}

        void CreateDescriptorSetPool(std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet,  uint32_t MAX_FRAMES);

        //takes input of descriptor set references(to fill out) and creates them from a list of descriptorset layouts
        void AllocateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
        void AllocateDescriptorSet(VkDescriptorSet& descriptorSet, VkDescriptorSetLayout& descriptorSetLayout);

        //descriptorBindingsPerSet is in [set][binding] form
        //each element of descriptorBindingsPerSet has the bindings
        //this loop takes a collection of bindings and creates a DescriptorSetLayout from them
        void CreateDescriptorSetLayout(std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet,std::vector<VkDescriptorSetLayout>& descriptorSetLayoutsOut);
        void resetPool();

        //update a descriptor set
        //if you are requesting multiple copies(as in per frame) allocate the copies field
        //pass in bindings for binding information for the set
        //set index is the descriptor set index
        // void UpdateDescriptorSet(std::vector<BufferResource*> descriptorBindingBufferResources,
        //                          VkDescriptorSet descriptorSet,
        //                          uint32_t setIndex,
        //                          VkDescriptorSetLayout descriptorSetLayout,
        //                          std::vector<VkDescriptorSetLayoutBinding>& descriptorBindings,
        //                          uint32_t copies);
    

        void Cleanup(); // explicit destruction


    protected:

        std::shared_ptr<GPU> gpu;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::vector<std::vector<std::string>> descriptorNamesPerSet;
        

    private:

 
};