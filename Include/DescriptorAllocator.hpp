#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <memory>
#include "GPU.hpp"

class DescriptorAllocator {
public:
    DescriptorAllocator() = delete;
    DescriptorAllocator(std::shared_ptr<GPU> gpu);
    ~DescriptorAllocator();

    void CreatePool(uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes);
    VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout layout);
    void resetPool();

    void destroy(); // explicit destruction

private:
    std::shared_ptr<GPU> gpu;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    
};