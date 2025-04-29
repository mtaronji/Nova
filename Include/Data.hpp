#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <array>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <cstring>
#include "GPU.hpp"

//a binding is another way of saying a memory allocation or data stream
//if you set everything in the struct below for one data stream, if pos changes and needs updating, you have to update color again
//much more efficient to have memory bindings for all the pieces of the vertex indivisually

//remember all this data is specifying how data is coming into the pipeline into the vertex shader. 
//for example, with this vertex data
//Vertex vertices[] = {
//     // Position           // Normal           // Color
//     { glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
//     { glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) },
//     { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) }
// };

//I can handle it a few ways. 
//1st way is to create a struct with a vec3 called position, a vec3 called normal and a vec4 called color
//then I can keep it with one binding and in the attribute part create for locations from the 1 binding.
//another way is to create 3 bindings. 1 for position, 1 for normal and 1 for color
//What you cannot do is have 1 binding and reusue the same binding
//remember, we want to send 

//VkVertexInputBindingDescription bindingDescription{};
//         bindingDescription.binding = 0;
//         bindingDescription.stride = sizeof(Vertex);
//         bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

//         return bindingDescription;
//     }

//     static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
//         std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

//         attributeDescriptions[0].binding = 0;
//         attributeDescriptions[0].location = 0;
//         attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
//         attributeDescriptions[0].offset = offsetof(Vertex, pos);

//         attributeDescriptions[1].binding = 0;
//         attributeDescriptions[1].location = 1;
//         attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
//         attributeDescriptions[1].offset = offsetof(Vertex, color);

    
struct alignas(16) VertexP{

    glm::vec3 pos;
    static uint32_t GetFieldOffset(int location){
        if(location == 0){
            return offsetof(VertexP, pos);
        }
        throw std::runtime_error("Incorrect Location specified for Vertexp : " + location);
    }
};
struct alignas(16) VertexPC{

    glm::vec3 pos;
    glm::vec4 color;
    static uint32_t GetFieldOffset(int location){
        if(location == 0) return offsetof(VertexPC,pos); 
        if(location == 1) return offsetof(VertexPC,color);

        throw std::runtime_error("Incorrect Location specified for Vertexpc : " + location);
    }
};
struct alignas(16) VertexPN{
    
    glm::vec3 pos;
    glm::vec3 normal;
    static uint32_t GetFieldOffset(int location){
        if(location == 0) return offsetof(VertexPN,pos); 
        if(location == 1) return offsetof(VertexPN,normal);
        throw std::runtime_error("Incorrect Location specified for Vertexpn: " + location);
    }
};
struct alignas(16) VertexPNT{
    
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
    static uint32_t GetFieldOffset(int location){
        if(location == 0) return offsetof(VertexPNT,pos); 
        if(location == 1) return offsetof(VertexPNT,normal);
        if(location == 2) return offsetof(VertexPNT,texCoord);
        throw std::runtime_error("Incorrect Location specified for Vertexpnt : " + location);
    }
};

struct alignas(16) VertexPT{

    glm::vec3 pos;
    glm::vec2 texCoord;
    static uint32_t GetFieldOffset(int location){
        if(location == 0) return offsetof(VertexPT,pos); 
        if(location == 1) return offsetof(VertexPT,texCoord);
        throw std::runtime_error("Incorrect Location specified for Vertexpt : " + location);
    }
};

struct alignas(16) VertexPNTC{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec4 color;
    static uint32_t GetFieldOffset(int location){
        if(location == 0) return offsetof(VertexPNTC,pos); if(location == 1) return offsetof(VertexPNTC,normal);
        if(location == 2) return offsetof(VertexPNTC,texCoord);if(location == 3) return offsetof(VertexPNTC,color);
        throw std::runtime_error("Incorrect Location specified for Vertexpntc : " + location);
    }
};



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
    template
    <typename T>
    static void CreateVertexBuffer(GPU& gpu, std::vector<T> vertices, VkBuffer& vBuffer, VkDeviceMemory &vMemory){

        auto device = gpu.GetVkDevice();
        assert(vertices.size() > 0);

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(gpu, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        CreateBuffer(gpu, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vBuffer, vMemory);

        CopyBuffer(gpu,stagingBuffer, vBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
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

