#include <vulkan/vulkan.h>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <cstring>
#include "GPU.hpp"


struct BufferOps{
 
    static void CreateBuffer(GPU& gpu, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {

        auto device = gpu.GetVkDevice();
        auto physicalDevice = gpu.GetPhysicalDevice();
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
        
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }

    static void CopyBuffer(GPU& gpu, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        auto device = gpu.GetVkDevice();
        auto commandPool = gpu.GetCommandPool();
        auto queue = gpu.GetGraphicsQueue();
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands(gpu);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(gpu,commandBuffer);
    }

    template <typename T>
    static void CreateDeviceBuffer(
        GPU& gpu,
        const std::vector<T>& data,
        VkBufferUsageFlags usage,
        VkBuffer& buffer,
        VkDeviceMemory& memory) {

        assert(!data.empty());
        VkDeviceSize bufferSize = sizeof(T) * data.size();
        auto device = gpu.GetVkDevice();

        // Create staging buffer (host-visible)
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;
        CreateBuffer(
            gpu,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingMemory
        );

        // Upload data
        void* mapped;
        vkMapMemory(device, stagingMemory, 0, bufferSize, 0, &mapped);
        memcpy(mapped, data.data(), (size_t)bufferSize);
        vkUnmapMemory(device, stagingMemory);

        // Create device-local buffer (actual GPU resource)
        CreateBuffer(
            gpu,
            bufferSize,
            usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // Enable transfer dst
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            buffer,
            memory
        );

        // Copy staging -> device
        CopyBuffer(gpu, stagingBuffer, buffer, bufferSize);

        // Cleanup
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingMemory, nullptr);
    }
    static VkCommandBuffer BeginSingleTimeCommands(GPU& gpu) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = gpu.GetCommandPool();
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(gpu.GetVkDevice(), &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    static void EndSingleTimeCommands(GPU& gpu,VkCommandBuffer commandBuffer) {
        auto graphicsQueue = gpu.GetGraphicsQueue();
        auto device = gpu.GetVkDevice();
        auto commandPool = gpu.GetCommandPool();
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }
    static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
};