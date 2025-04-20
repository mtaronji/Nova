#pragma once

#include <vulkan/vulkan.h>
#include "RenderPassManager.hpp"
#include "SwapchainManager.hpp"
#include "PipelineManager.hpp"
#include <vector>
#include <stdexcept>

class CommandManager {
    public:
        CommandManager();
        CommandManager(uint32_t framecount);
        void Init(VkDevice device, uint32_t queueFamilyIndex);
        void Cleanup();

        VkCommandPool GetCommandPool() const { return commandPool; }
        const std::vector<VkCommandBuffer>& GetCommandBuffers() const { return commandBuffers; }
        const VkCommandBuffer GetCommandBuffer(uint32_t frame) const {return commandBuffers[frame];}
        uint32_t GetFrameCount()const {return FRAME_COUNT;}

        void AllocateCommandBuffers();
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue graphicsQueue);
        void RecordGraphicsPipelineCommandBuffer(VkCommandBuffer commandBuffer, uint32_t currentFrame, RenderPassManager& renderPassManager, PipelineManager& pipelinemanager);

        class Builder{

            //buildCommand buffer
            public:
                Builder& BeginCommandBufferCmd(VkCommandBuffer& commandBuffer, VkCommandBufferBeginInfo beginInfo){
                    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
                        throw std::runtime_error("failed to begin recording command buffer!");
                    }
                }
                Builder& BeginRenderPassCmd(){
                    
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

    protected:
        VkDevice device = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers;
        const uint32_t FRAME_COUNT;
};
