#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include "GPU.hpp"

class DescriptorAllocator {
    public:
        DescriptorAllocator() = delete;
        DescriptorAllocator(std::shared_ptr<GPU> gpu);
        ~DescriptorAllocator();

        VkDescriptorPool GetPool()const {return descriptorPool;}
        std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts () const {return descriptorSetLayouts;}
        std::vector<VkDescriptorSet> GetDescriptorSets () const {return descriptorSets;}

        void CreateDescriptorSetPool(uint32_t maxSets, const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet);
        void AllocateDescriptorSets();
        void CreateDescriptorSetLayout(const std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorBindingsPerSet);
        void resetPool();

        void destroy(); // explicit destruction

    protected:
        std::shared_ptr<GPU> gpu;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> descriptorSets;
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

    private:
        const uint32_t MAX_FRAMES = 3;

 
};