#pragma once

#include <vulkan/vulkan.h>
#include "RenderPassManager.hpp"
#include "SwapchainManager.hpp"
#include "Pipeline.hpp"
#include <vector>
#include <stdexcept>

class CommandManager {
public:
    CommandManager();
    void Init(VkDevice device, uint32_t queueFamilyIndex);
    void Cleanup();

    VkCommandPool GetCommandPool() const { return commandPool; }
    const std::vector<VkCommandBuffer>& GetCommandBuffers() const { return commandBuffers; }

    void AllocateCommandBuffers(uint32_t count);
    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue graphicsQueue);
    void RecordGraphicsPipelineCommandBuffer(VkCommandBuffer commandBuffer, uint32_t currentFrame, RenderPassManager& renderPassManager, Pipeline);

    class Builder{

        //buildCommand buffer
        public:
            Builder& BeginCommandBufferCmd(VkCommandBuffer& commandBuffer, VkCommandBufferBeginInfo beginInfo){
                if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
                    throw std::runtime_error("failed to begin recording command buffer!");
                }
            }
            Builder& BeginRenderPassCmd(VkRenderPassBeginInfo renderPassInfo, VkFramebuffer framebuffer){
                vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            }
            Builder& BindGraphicsPipelineCmd();
            Builder& BindViewportCmd();
            Builder& BindScissorCmd();
            Builder& BindVertexBufferCmd();
            Builder& BindIndexBufferCmd();
            Builder& BindDescriptorSetsCmd();
            Builder& DrawIndexedCmd();

        private:

    };

private:
    VkDevice device = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;
};
