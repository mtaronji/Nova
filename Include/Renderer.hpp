#pragma once

#include "RenderPassManager.hpp"
#include "PipelineManager.hpp"
#include "GPU.hpp"
#include "SwapchainManager.hpp"
#include "SyncManager.hpp"
#include "CommandManager.hpp"
#include "VulkanEngine.hpp"
#include <array>
#include <cassert>

class Renderer {
    public:
        Renderer(GPU& gpu, 
            VulkanEngine &engine,
            SwapchainManager& swapchainmanager, 
            SyncManager& syncsmanager, 
            RenderPassManager& renderpassmanager,
            const PipelineManager& pipelinemanager, 
            CommandManager& commandmanager
        );
        Renderer() = delete;
        ~Renderer();

        void virtual DrawFrame();
        
        void NotifySwapchainOutOfDate(); 
        
        class Builder{
            
            Builder& SetGPU(GPU& gpu) 
            { 
                this->gpu = &gpu;
                return *this;
            }
            Builder& SetVulkanEngine(VulkanEngine& engine){
                this->engine = &engine;
                return *this;
            }
            Builder& SetSwapChainManager(SwapchainManager& swapchainmanager){
                this->swapchainmanager = &swapchainmanager;
                return *this;
            }
            Builder& SetSyncManager(SyncManager& syncmanager){
                this->syncmanager = &syncmanager;
                return *this;
            }
            Builder& SetPipelineManager(PipelineManager& pipelinemanager){
                this->pipelinemanager = &pipelinemanager;
                return *this;
            }
            Builder& SetRenderpassManager(RenderPassManager& renderpassmanager){
                this->renderpassmanager = &renderpassmanager;
                return *this;
            }
            Builder& CreateCommandBuffers(CommandManager& commandmanager){
                this->commandmanager = &commandmanager;
                return *this;
            }
            Builder& CreateFrameBuffers(){
                
            }
            Renderer Build(){
                return Renderer(*gpu, *engine,*swapchainmanager, *syncmanager, *renderpassmanager, *pipelinemanager, *commandmanager);
            }
            private:
                GPU*  gpu;
                VulkanEngine* engine;
                SwapchainManager* swapchainmanager;
                SyncManager* syncmanager;
                PipelineManager* pipelinemanager;
                RenderPassManager* renderpassmanager;
                CommandManager* commandmanager;
                std::vector<VkFramebuffer>* framebuffers;


        };
    protected:
        void virtual DrawFrameCommands(VkCommandBuffer commandBuffer, 
                                        uint32_t imageIndex, 
                                        VkBuffer vbuffers[] = VK_NULL_HANDLE,
                                        VkBuffer ibuffer = VK_NULL_HANDLE);
        GPU& gpu;
        VulkanEngine& engine;
        SwapchainManager& swapchainmanager;
        SyncManager& syncsmanager;
        RenderPassManager& renderpassmanager;
        const PipelineManager& pipelinemanager;
        CommandManager& commandmanager;
        std::vector<VkFramebuffer> framebuffers;


        uint32_t currentFrame = 0;
        static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;

        bool framebufferResized = false;

        void RecreateSwapchain();
        void DestroyFrameBuffers();
        void CreateFrameBuffers();

};