#pragma once

#include "RenderPassManager.hpp"
#include "PipelineManager.hpp"
#include "GPU.hpp"
#include "SwapchainManager.hpp"
#include "SyncManager.hpp"
#include "CommandManager.hpp"
#include "VulkanEngine.hpp"
#include "FramebufferGenerator.hpp"
#include <array>
#include <cassert>
#include <memory>
#include "Data.hpp"

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

        void virtual DrawFrame();
        
        void NotifySwapchainOutOfDate(); 
        
        class Builder{
            Builder& WithShell(std::shared_ptr<Shell> shell) 
            { 
                this->shell = shell;
                return *this;
            }
            Builder& WithGPU(std::shared_ptr<GPU> gpu) 
            { 
                this->gpu = gpu;
                return *this;
            }
            Builder& WithVulkanEngine(std::shared_ptr<VulkanEngine>){
                this->engine = engine;
                return *this;
            }
            Builder& WithSwapChainManager(std::shared_ptr<SwapchainManager> swapchainmanager){
                this->swapchainmanager = swapchainmanager;
                return *this;
            }
            Builder& WithSyncManager(std::shared_ptr<SyncManager> syncmanager){
                this->syncmanager = syncmanager;
                return *this;
            }
            Builder& WithPipelineManager(std::shared_ptr<PipelineManager> pipelinemanager){
                this->pipelinemanager = pipelinemanager;
                return *this;
            }
            Builder& WithRenderpassManager( std::shared_ptr<RenderPassManager> renderpassmanager){
                this->renderpassmanager = renderpassmanager;
                return *this;
            }
            Builder& WithCommandManager(std::shared_ptr<CommandManager> commandmanager){
                this->commandmanager = commandmanager;
                return *this;
            }
            Builder& WithFrameBuffers(std::shared_ptr<FramebufferGenerator> generator){
                this->framebufferContainer = std::make_shared<FramebufferGenerator>(gpu->GetVkDevice(), 
                                                                  renderpassmanager->GetRenderPass(), 
                                                                  swapchainmanager);
                return *this;
            }
            Renderer Build(){
                return Renderer(shell,gpu, engine,swapchainmanager, syncmanager, pipelinemanager, renderpassmanager,  commandmanager, framebufferContainer);
            }
            private:
                std::shared_ptr<GPU> gpu;
                std::shared_ptr<VulkanEngine> engine;
                std::shared_ptr<SwapchainManager> swapchainmanager;
                std::shared_ptr<SyncManager> syncmanager;
                std::shared_ptr<PipelineManager> pipelinemanager;
                std::shared_ptr<RenderPassManager> renderpassmanager;
                std::shared_ptr<CommandManager> commandmanager;
                std::shared_ptr<FramebufferGenerator> framebufferContainer;
                std::shared_ptr<Shell> shell;



        };
    protected:
        void virtual DrawFrameCommands(VkCommandBuffer commandBuffer, 
                                        uint32_t imageIndex, 
                                        VkBuffer vbuffers[] = VK_NULL_HANDLE,
                                        VkBuffer ibuffer = VK_NULL_HANDLE);

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