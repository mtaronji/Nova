#pragma once

#include <fstream>
#include <stdexcept>
#include <vector>
#include <set>
#include <memory>
#include <optional>
#include <vulkan/vulkan.h>
#include <vector>

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
class VulkanEngine;
class GPU {
    public:
        GPU(std::shared_ptr<VulkanEngine> engine);
        void Cleanup();

        VkDevice GetVkDevice() const { return device; }
        VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
        VkQueue GetGraphicsQueue() const { return graphicsQueue; }
        VkQueue GetPresentQueue() const { return presentQueue; }
        VkCommandPool GetCommandPool()const{return commandPool;}

        uint32_t GetGraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }
        uint32_t GetPresentQueueFamilyIndex() const { return presentQueueFamilyIndex; }
        QueueFamilyIndices FindQueueFamilies();

    protected:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;
        uint32_t graphicsQueueFamilyIndex = 0;
        uint32_t presentQueueFamilyIndex = 0;
        std::shared_ptr<VulkanEngine> engine;
        VkCommandPool commandPool;

        void PickPhysicalDevice(VkInstance instance);
        void CreateLogicalDevice();
        void CreateCommandPool();
        const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
        };
};