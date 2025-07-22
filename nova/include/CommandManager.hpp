#pragma once

#include <vulkan/vulkan.h>
#include "RenderPassManager.hpp"
#include "SwapchainManager.hpp"
#include "PipelineManager.hpp"
#include "GPU.hpp"

#include <vector>
#include <memory>
#include <stdexcept>

class CommandManager {
    public:
        CommandManager() = delete;
        CommandManager(std::shared_ptr<GPU> gpu, uint32_t MAX_FRAMES):gpu(gpu),MAX_FRAMES(MAX_FRAMES){}

        void CreateCommandPool();
        void Init();
        void Cleanup();

        VkCommandPool GetCommandPool() const { return commandPool; }
        const std::vector<VkCommandBuffer>& GetCommandBuffers() const { return commandBuffers; }
        const VkCommandBuffer GetCommandBuffer(uint32_t frame) const {return commandBuffers[frame];}
        uint32_t GetFrameCount()const {return MAX_FRAMES;}

        void AllocateCommandBuffers();
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
        void RecordGraphicsPipelineCommandBuffer(VkCommandBuffer commandBuffer, uint32_t currentFrame, RenderPassManager& renderPassManager, PipelineManager& pipelinemanager);


    protected:
        std::shared_ptr<GPU> gpu;

        VkCommandPool commandPool = VK_NULL_HANDLE; //graphics pool
        std::vector<VkCommandBuffer> commandBuffers;
        VkCommandBuffer SingleCommand;

    private:
        uint32_t MAX_FRAMES;
};
