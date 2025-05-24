#pragma once
#include "IRenderLoopClient.hpp"
#include "UBOs.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

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
class ResourceManager;
struct Mesh;

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
            std::shared_ptr<DescriptorAllocator> descriptorAllocator,
            std::shared_ptr<ResourceManager> resourceManager
        );
        ~Nova();
        
        void Start() override;
        
        void Init() override;
        void Update(float deltaTime) override;
        void Render() override;
        void Shutdown() override;

        class Builder{
            public:
                Builder() = default;
                std::unique_ptr<IRenderLoopClient> Build();

         
                virtual Builder& WithShell();                   Builder& WithShell(std::shared_ptr<Shell> shell){ this->shell = shell; return *this;}
                virtual Builder& WithEngine();                  Builder& WithEngine(std::shared_ptr<VulkanEngine> engine){this->engine = engine; return *this;}
                virtual Builder& WithGPU();                     Builder& WithGPU(std::shared_ptr<GPU> gpu){ this->gpu = gpu; return *this;}
                virtual Builder& WithSwapchainManager();        Builder& WithSwapchainManager(std::shared_ptr<SwapchainManager>swapchainManager){this->swapchainManager = swapchainManager; return *this;}
                virtual Builder& WithRenderpass();              Builder& WithRenderpass(std::shared_ptr<RenderPassManager> renderpassManager){this->renderpassManager = renderpassManager; return *this;}
                virtual Builder& WithFramebufferGenerator();    Builder& WithFramebufferGenerator(std::shared_ptr<FramebufferGenerator> framebuffersContainer){this->framebuffersContainer = framebuffersContainer; return *this;}
                virtual Builder& WithDescriptorAllocator();     Builder& WithDescriptorAllocator(std::shared_ptr<DescriptorAllocator> descriptorAllocator){this->descriptorAllocator = descriptorAllocator; return *this;}
                virtual Builder& WithPipelineManager();         Builder& WithPipelineManager(std::shared_ptr<PipelineManager> pipelineManager){this->pipelineManager = pipelineManager; return *this;}
                virtual Builder& WithSyncManager();             Builder& WithSyncManager(std::shared_ptr<SyncManager> syncManager){this->syncManager = syncManager; return *this;}
                virtual Builder& WithCommandManager();          Builder& WithCommandManager(std::shared_ptr<CommandManager> commandManager){this->commandManager = commandManager; return *this;}
                virtual Builder& WithPipelineLibrary();         Builder& WithPipelineLibrary(std::shared_ptr<PipelineLibrary> pipelineLibrary){ this->pipelineLibrary = pipelineLibrary; return *this;}
                virtual Builder& WithResourceManager();         Builder& WithResourceManager(std::shared_ptr<ResourceManager> resourceManager){ this->resourceManager = resourceManager; return *this;}
                virtual Builder& WithMeshes();                  Builder& WithMeshes(std::unordered_map<std::string, Mesh*> meshes);
                virtual Builder& WithDescriptorSets();          Builder& WithDescriptorSets(std::vector<std::vector<BufferResource*>>& descriptorSets);
                virtual Builder& WithResourceMap();             Builder& WithResourceMap(std::unordered_map<std::string, BufferResource*> resourceMap);
                
                
                

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
                std::shared_ptr<ResourceManager> resourceManager;

                CameraUBO camera = {};
                glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 5.0f);                  // Camera at z = 5
                glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.5f);            // Looking at center of square
                glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);           // Up direction
               
                
            private:
                std::vector<const char*> deviceExtensions = {
                    VK_KHR_SWAPCHAIN_EXTENSION_NAME                  // Required for presentation
                    // VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,         // Enables render pass 2 structures
                    // VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME // Enables separate depth/stencil layouts
                };                  
        };
    
    protected:
        std::shared_ptr<VulkanEngine> engine;
        std::shared_ptr<Shell> shell;
        std::shared_ptr<GPU> gpu;
        std::shared_ptr<FramebufferGenerator> framebuffersGenerator;
        std::shared_ptr<SyncManager> syncManager;
        std::shared_ptr<SwapchainManager>swapchainManager;
        std::shared_ptr<PipelineLibrary>pipelineLibrary;
        std::shared_ptr<PipelineManager> pipelineManager;
        std::shared_ptr<RenderPassManager> renderpassManager;
        std::shared_ptr<CommandManager> commandManager;
        std::unordered_map<std::string, Shader> shaders;
        std::shared_ptr<DescriptorAllocator> descriptorAllocator;
        std::shared_ptr<Renderer> renderer;
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        std::shared_ptr<ResourceManager> resourceManager;
        CameraUBO camera = {};
        glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 5.0f);                  // Camera at z = 5
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.5f);            // Looking at center of square
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);                // Up direction
        
        

        virtual void CreateMoniliths();
        virtual void AllocateToMonoliths();


    private:
        uint32_t MAX_FRAMES = 3;
        float angle = glm::pi<float>() * 1.5f;
        float cameraspeed = glm::pi<float>() / 20.0f;
        float orbitalDistance = 5.0f;    
};



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