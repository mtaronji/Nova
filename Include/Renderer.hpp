#pragma once

#include <vulkan/vulkan.h>
#include <array>
#include <cassert>
#include <memory>
#include <unordered_map>


class Shell;
class GPU;
class VulkanEngine;
class SwapchainManager;
class SyncManager;
class PipelineManager;
class RenderPassManager;
class CommandManager;
class FramebufferGenerator;
struct BufferResource;
class ResourceManager;

class Renderer {
    public:
        Renderer(   
            std::shared_ptr<Shell> shell,
            std::shared_ptr<GPU> gpu,
            std::shared_ptr<VulkanEngine> engine,
            std::shared_ptr<SwapchainManager> swapchainmanager,
            std::shared_ptr<SyncManager> syncmanager,
            std::shared_ptr<PipelineManager> pipelinemanager,
            std::shared_ptr<RenderPassManager> renderpassmanager,
            std::shared_ptr<CommandManager> commandmanager,
            std::shared_ptr<FramebufferGenerator> framebufferContainer
        );
        Renderer() = delete;
        ~Renderer();

        void virtual DrawFrame(ResourceManager* resourceManager);
        
        void NotifySwapchainOutOfDate(); 
        
    protected:
        void virtual DrawFrameCommands(VkCommandBuffer commandBuffer, 
                                        uint32_t imageIndex,
                                        ResourceManager* resourceManager
                                        );

        std::shared_ptr<GPU> gpu;
        std::shared_ptr<VulkanEngine> engine;
        std::shared_ptr<SwapchainManager> swapchainmanager;
        std::shared_ptr<SyncManager> syncmanager;
        std::shared_ptr<PipelineManager> pipelinemanager;
        std::shared_ptr<RenderPassManager> renderpassmanager;
        std::shared_ptr<CommandManager> commandmanager;
        std::shared_ptr<FramebufferGenerator> framebufferContainer;
        std::shared_ptr<Shell> shell;


        uint32_t currentFrame = 0;
        static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;

        bool framebufferResized = false;

        void RecreateSwapchain();
        void DestroyFrameBuffers();
        void CreateFrameBuffers();


};