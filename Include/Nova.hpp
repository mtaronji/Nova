#pragma once
#include "IRenderLoopClient.hpp"
#include "Shell.hpp"
#include "Renderer.hpp"
#include "VulkanEngine.hpp"
#include "GPU.hpp"
#include "PipelineConfig.hpp"
#include "PipelineLibrary.hpp"
#include "RenderPassManager.hpp"
#include "SwapchainManager.hpp"
#include "VulkanResourceManager.hpp"
#include "CommandManager.hpp"
#include "FramebufferManager.hpp"
#include "Shader.hpp"
#include "SyncManager.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>

class Nova : IRenderLoopClient{
    public:
    Nova() = delete;
    Nova(
        VulkanEngine &engine, 
        Shell& shell, 
        GPU& gpu, 
        FramebufferManager& framebufferManager, 
        SyncManager& syncManager,
        SwapchainManager& swapchainManager,
        PipelineLibrary& pipelineLibrary,
        RenderPassManager& renderpassManager,
        CommandManager& commandManager,
        std::unordered_map<std::string, Shader>& shaders);
        
        void Start();
        
        void Init() override;
        void Load() override;
        void Update(float deltaTime = 0.0f) override;
        void Render() override;
        void Shutdown() override;

        class Builder{
            public:
                Builder();
                Builder& CreateShell();
                Builder& CreateEngine();
                Builder& CreateGPU();
                Builder& CreateSwapchainManager();
                Builder& CreateRenderPassManager();
                Builder& CreateShaders();
                Builder& CreateSyncManagers();
                Builder& CreatePipelineManagers();
                Nova Build();
            protected:
                VulkanEngine* engine;
                Shell* shell;
                GPU* gpu;
                FramebufferManager* framebufferManager;
                SyncManager* syncManager;
                SwapchainManager* swapchainManager;
                PipelineLibrary* pipelineLibrary;
                RenderPassManager* renderpassManager;
                CommandManager* commandManager;
                std::vector<AttachmentInfo> attachmentInfos;
                std::unordered_map<std::string, Shader> shaders;
            private:
                void InitAttachments(std::vector<AttachmentInfo>& attachmentInfos);


        };
    
    protected:
        VulkanEngine& engine;
        Shell& shell;
        GPU& gpu;
        FramebufferManager& framebufferManager;
        SyncManager& syncManager;
        SwapchainManager& swapchainManager;
        PipelineLibrary& pipelineLibrary;
        RenderPassManager& renderpassManager;
        CommandManager& commandManager;
        std::unordered_map<std::string, Shader> shaders;
        bool initialized = false;

    private:
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        
};