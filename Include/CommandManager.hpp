#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

class CommandManager {
public:
    void Init(VkDevice device, uint32_t queueFamilyIndex);
    void Cleanup();

    VkCommandPool GetCommandPool() const { return commandPool; }
    const std::vector<VkCommandBuffer>& GetCommandBuffers() const { return commandBuffers; }

    void AllocateCommandBuffers(uint32_t count);
    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue graphicsQueue);

private:
    VkDevice device = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;
};
