#include <fstream>
#include <stdexcept>
#include <vector>

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanEngine.hpp"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class GPU {
public:
    void Create(VulkanEngine& instance);
    void Cleanup();

    VkDevice GetVkDevice() const { return device; }
    VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
    VkQueue GetGraphicsQueue() const { return graphicsQueue; }
    VkQueue GetPresentQueue() const { return presentQueue; }
    uint32_t GetGraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }
    uint32_t GetGraphicsQueueFamilyIndex() const { return presentQueueFamilyIndex; }
    QueueFamilyIndices GPU::FindQueueFamilies(const VkSurfaceKHR& surface);

private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamilyIndex = 0;
    uint32_t presentQueueFamilyIndex = 0;

    void PickPhysicalDevice(VkInstance instance);
    void CreateLogicalDevice();

    const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
    };
};