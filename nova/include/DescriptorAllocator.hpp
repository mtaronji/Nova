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


        void CreateDescriptorSetPool(std::vector<VkDescriptorPoolSize>&, uint32_t, VkDescriptorPoolCreateFlags);

        //takes input of descriptor set references(to fill out) and creates them from a list of descriptorset layouts
        void AllocateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
        void AllocateDescriptorSet(VkDescriptorSet& descriptorSet, VkDescriptorSetLayout& descriptorSetLayout);

        //descriptorBindingsPerSet is in [set][binding] form
        //each element of descriptorBindingsPerSet has the bindings
        //this loop takes a collection of bindings and creates a DescriptorSetLayout from them
        void CreateDescriptorSetLayout(std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet,std::vector<VkDescriptorSetLayout>& descriptorSetLayoutsOut);
        void resetPool();
    
        void Cleanup(); // explicit destruction


    protected:

        std::shared_ptr<GPU> gpu;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::vector<std::vector<std::string>> descriptorNamesPerSet;
        bool dynamic = false;
        

    private:

 
};