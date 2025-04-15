#include <fstream>
#include <stdexcept>
#include <vector>

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanEngine.hpp"

class GPU {
public:
    void Create(VulkanEngine& instance);
    void Cleanup();

    VkDevice GetVkDevice() const { return device; }
    VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
    VkQueue GetGraphicsQueue() const { return graphicsQueue; }
    uint32_t GetGraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }

private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamilyIndex = 0;

    void PickPhysicalDevice(VkInstance instance);
    void CreateLogicalDevice();
};