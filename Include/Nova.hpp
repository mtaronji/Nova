#pragma once
#include "IRenderLoopClient.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <memory>



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
class Renderer;
class PipelineLibrary; 
class DescriptorAllocator;
class Shader;
class Nova : public IRenderLoopClient{
    public:
        Nova() = delete;
        Nova(
            std::shared_ptr<VulkanEngine> engine,
            std::shared_ptr<Shell> shell,
            std::shared_ptr<GPU> gpu,
            std::shared_ptr<FramebufferGenerator> framebuffersContainer,
            std::shared_ptr<SyncManager> syncManager,
            std::shared_ptr<SwapchainManager>swapchainManager,
            std::shared_ptr<PipelineManager> pipelineManager,
            std::shared_ptr<PipelineLibrary>pipelineLibrary,
            std::shared_ptr<RenderPassManager> renderpassManager,
            std::shared_ptr<CommandManager> commandManager,
            std::shared_ptr<DescriptorAllocator> descriptorAllocator
        );
        
        void Start() override;
        
        void Init() override;
        void LoadResourceMap(std::unordered_map<std::string, BufferResource>* resourceMap) override;
        void Update(float deltaTime) override;
        void Render() override;
        void Shutdown() override;



        class Builder{
            public:
                Builder() = default;
                std::unique_ptr<IRenderLoopClient> Build();

         
                Builder& WithShell();                   Builder& WithShell(std::shared_ptr<Shell> shell){ this->shell = shell; return *this;}
                Builder& WithEngine();                  Builder& WithEngine(std::shared_ptr<VulkanEngine> engine){this->engine = engine; return *this;}
                Builder& WithGPU();                     Builder& WithGPU(std::shared_ptr<GPU> gpu){ this->gpu = gpu; return *this;}
                Builder& WithSwapchainManager();        Builder& WithSwapchainManager(std::shared_ptr<SwapchainManager>swapchainManager){this->swapchainManager = swapchainManager; return *this;}
                Builder& WithRenderpass();              Builder& WithRenderpass(std::shared_ptr<RenderPassManager> renderpassManager){this->renderpassManager = renderpassManager; return *this;}
                Builder& WithFramebufferGenerator();    Builder& WithFramebufferGenerator(std::shared_ptr<FramebufferGenerator> framebuffersContainer){this->framebuffersContainer = framebuffersContainer; return *this;}
                Builder& WithDescriptorAllocator();     Builder& WithDescriptorAllocator(std::shared_ptr<DescriptorAllocator> descriptorAllocator){this->descriptorAllocator = descriptorAllocator; return *this;}
                Builder& WithPipelineManager();         Builder& WithPipelineManager(std::shared_ptr<PipelineManager> pipelineManager){this->pipelineManager = pipelineManager; return *this;}
                Builder& WithSyncManager();             Builder& WithSyncManager(std::shared_ptr<SyncManager> syncManager){this->syncManager = syncManager; return *this;}
                Builder& WithCommandManager();          Builder& WithCommandManager(std::shared_ptr<CommandManager> commandManager){this->commandManager = commandManager; return *this;}
                Builder& WithPipelineLibrary();         Builder& WithPipelineLibrary(std::shared_ptr<PipelineLibrary> pipelineLibrary){ this->pipelineLibrary = pipelineLibrary; return *this;}


            protected:
                std::shared_ptr<Shell> shell;
                std::shared_ptr<VulkanEngine> engine;
                std::shared_ptr<GPU> gpu;

                std::shared_ptr<SwapchainManager>swapchainManager;

                std::shared_ptr<RenderPassManager> renderpassManager;
                std::shared_ptr<FramebufferGenerator> framebuffersContainer;

                std::shared_ptr<DescriptorAllocator> descriptorAllocator;                               
                std::shared_ptr<PipelineManager> pipelineManager;
                
                std::shared_ptr<SyncManager> syncManager;

                std::shared_ptr<PipelineLibrary>pipelineLibrary;

                std::shared_ptr<CommandManager> commandManager;

                
                
            private:
                
                

        };
    
    protected:
        std::shared_ptr<VulkanEngine> engine;
        std::shared_ptr<Shell> shell;
        std::shared_ptr<GPU> gpu;
        std::shared_ptr<FramebufferGenerator> framebuffersContainer;
        std::shared_ptr<SyncManager> syncManager;
        std::shared_ptr<SwapchainManager>swapchainManager;
        std::shared_ptr<PipelineLibrary>pipelineLibrary;
        std::shared_ptr<PipelineManager> pipelineManager;
        std::shared_ptr<RenderPassManager> renderpassManager;
        std::shared_ptr<CommandManager> commandManager;
        std::unordered_map<std::string, Shader> shaders;
        std::shared_ptr<DescriptorAllocator> descriptorAllocator;
        std::shared_ptr<Renderer> renderer;
           

        bool initialized = false;



    private:
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        uint32_t MAX_FRAMES = 3;

        
};


//         createIndexBuffer();
//         createUniformBuffers();
//         createDescriptorPool();
//         createDescriptorSets();
//         createCommandBuffers();
//         createSyncObjects();

/*
🛠️ 1. Create the Vulkan Instance
vkCreateInstance(...)

Include extensions (e.g., VK_KHR_surface, validation layers)

Enable validation layers (e.g., VK_LAYER_KHRONOS_validation)

Optionally set up a debug messenger via VK_EXT_debug_utils

🖼️ 2. Create a Window (Platform-Specific)
Use something like GLFW or SDL to create a window

Create a Vulkan-compatible surface (vkCreateWin32SurfaceKHR, vkCreateXcbSurfaceKHR, etc.)

🧱 3. Pick a Physical Device (GPU)
Use vkEnumeratePhysicalDevices

Choose one that supports required queue families, extensions, and features

Needs to support presenting to your surface

Needs a graphics queue

🔧 4. Create a Logical Device + Queues
Use vkCreateDevice(...)

Request graphics and present queues

Get handles via vkGetDeviceQueue(...)

🪞 5. Create a Swapchain
Use VK_KHR_swapchain extension

Choose:

Surface format (color depth, e.g., VK_FORMAT_B8G8R8A8_UNORM)

Present mode (e.g., FIFO, MAILBOX)

Extent (matches window size)

Create image views for each image in the swapchain

 **Create descriptor set layout(s) ← 🎯 Should be here

📦 6. Create a Render Pass
Define attachments (e.g., color, depth)

Set subpasses and dependencies

This describes what the GPU will render to

📐 7. Create Framebuffers
One framebuffer per swapchain image

Connect image views to the attachments in your render pass

🔼 8. Create Command Pool
Based on the graphics queue family

Used to allocate command buffers

🧾 9. Create Command Buffers
Allocate from the command pool

You’ll record draw commands into these each frame

🖼️ 10. Create Sync Objects
Semaphores for imageAvailable and renderFinished

Fences to wait on GPU completion per frame (for multiple frames in flight)

🧠 11. Create Pipeline & Shaders
Compile shaders to SPIR-V (e.g., with glslc)

Create shader modules

Configure:

Input layout (vertex binding/attribute descriptions)

Viewports, scissors

Rasterizer, color blend, depth stencil

Build your graphics pipeline

🪣 12. Create Vertex Buffers / Index Buffers / Uniforms
Set up memory allocation

Use vkCmdCopyBuffer, vkMapMemory, vkFlushMappedMemoryRanges, etc.

🗺️ 13. Create Descriptor Set Layouts & Pools
Define what your shaders will access (e.g., uniforms, textures)

Allocate and bind descriptor sets





*/