#pragma once

#include "RenderPassManager.hpp"
#include "Pipeline.hpp"
#include "GPU.hpp"
#include "SwapchainManager.hpp"
#include "SyncManager.hpp"
#include "CommandManager.hpp"
#include <array>


class Renderer {
    public:
        Renderer(const GPU& gpu, SwapchainManager& swapchain, SyncManager& syncs, const Pipeline& pipeline, std::vector<VkCommandBuffer>& commandBuffers);
        Renderer() = delete;
        ~Renderer();

        void virtual DrawFrame();
        
        void NotifySwapchainOutOfDate(); 
        
        class Builder{
            
            Builder& SetGPU(GPU& gpu) 
            { 
                this->gpu = gpu;
                return *this;
            }

            Builder& SetSwapChain(SwapchainManager& swapchain){
                this->swapchain = swapchain;
                return *this;
            }
            Builder& SetSyncs(SyncManager&syncs){
                this->syncs = syncs;
                return *this;
            }
            Builder& SetPipeline(Pipeline& pipeline){
                this->syncs = syncs;
                return *this;
            }
            Builder& CreateCommandBuffers(CommandManager& commandmanager){
                this->commandmanager = commandmanager;
                return *this;
            }
            Builder& CreateFrameBuffers();
            Renderer Build(){
                return Renderer(gpu, swapchain, syncs, pipeline,commandBuffers);
            }
            private:
                GPU&  gpu;
                SwapchainManager& swapchain;
                SyncManager& syncs;
                Pipeline& pipeline;
                CommandManager& commandmanager;
                std::vector<VkCommandBuffer>& commandBuffers;
                std::vector<VkFramebuffer> swapChainFramebuffers;

        };
    protected:
        void virtual DrawFrameCommands();

    private:
        const GPU& gpu;
        SwapchainManager& swapchain;
        SyncManager& syncs;
        const Pipeline& pipeline;
        const std::vector<VkCommandBuffer>& commandBuffers;

        uint32_t currentFrame = 0;
        static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;

        bool framebufferResized = false;

};