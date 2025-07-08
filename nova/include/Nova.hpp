#pragma once

class FramebufferLibrary;
class CommandManager;
class Shell;
class GPU;
class VulkanEngine;
class SyncManager;
class SwapchainManager;
class PipelineLibrary;
class RenderpassLibrary;
class DescriptorAllocator;
class ResourceManager;
class VulkanEngine;
class Renderer;
class Mesh;
class BufferResource;
class Shader;


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <set>
#include <glm/gtc/constants.hpp>
#include <filesystem>
#include <functional>
#include <unordered_set>
#include "UBOs.hpp"
#include "IRenderLoopClient.hpp"
#include "DescriptorsetLoader.hpp"
#include "MouseEvents.hpp"
#include "KeyboardEvents.hpp"

#define STB_IMAGE_IMPLEMENTATION


class Nova : public IRenderLoopClient{
    public:
        Nova() = delete;
        Nova(
            std::shared_ptr<VulkanEngine> engine,
            std::shared_ptr<Shell> shell,
            std::shared_ptr<GPU> gpu,
            std::shared_ptr<FramebufferLibrary> framebuffersLibrary,
            std::shared_ptr<SyncManager> syncManager,
            std::shared_ptr<SwapchainManager>swapchainManager,
            std::shared_ptr<PipelineLibrary>pipelineLibrary,
            std::shared_ptr<RenderpassLibrary> renderpassLibrary,
            std::shared_ptr<CommandManager> commandManager,
            std::shared_ptr<DescriptorAllocator> descriptorAllocator,
            std::shared_ptr<ResourceManager> resourceManager,
            std::unordered_map<std::string, DescriptorFile> descriptorFiles
        );
        ~Nova();
        
        virtual void Start() override;   
        virtual void Init() override;
        virtual void Update(float deltaTime) override;
        virtual void Render() override;
        virtual void Shutdown() override;

        //observers to hook into events
        void ObserveMouseButton(std::function<void(MouseButtonEvent)> observer);
        void ObserveMouseLocation(std::function<void(MouseMoveEvent)> observer); 
        void ObserveKeyPress(std::function<void(KeyPressEvent)> observer); 



        class Builder{
            public:
                Builder() = default;
                std::unique_ptr<IRenderLoopClient> Build();

         
                virtual Builder& WithShell();                   Builder& WithShell(std::shared_ptr<Shell> shell){ this->shell = shell; return *this;}
                virtual Builder& WithEngine();                  Builder& WithEngine(std::shared_ptr<VulkanEngine> engine){this->engine = engine; return *this;}
                virtual Builder& WithGPU();                     Builder& WithGPU(std::shared_ptr<GPU> gpu){ this->gpu = gpu; return *this;}
                virtual Builder& WithSwapchainManager();        Builder& WithSwapchainManager(std::shared_ptr<SwapchainManager>swapchainManager){this->swapchainManager = swapchainManager; return *this;}
                virtual Builder& WithDescriptorAllocator();     Builder& WithDescriptorAllocator(std::shared_ptr<DescriptorAllocator> descriptorAllocator){this->descriptorAllocator = descriptorAllocator; return *this;}
                virtual Builder& WithSyncManager();             Builder& WithSyncManager(std::shared_ptr<SyncManager> syncManager){this->syncManager = syncManager; return *this;}
                virtual Builder& WithCommandManager();          Builder& WithCommandManager(std::shared_ptr<CommandManager> commandManager){this->commandManager = commandManager; return *this;}
                virtual Builder& WithRenderpassLibrary();       Builder& WithRenderpassLibrary(std::shared_ptr<RenderpassLibrary> renderpassLibrary){ this->renderpassLibrary = renderpassLibrary; return *this;}
                virtual Builder& WithPipelineLibrary();         Builder& WithPipelineLibrary(std::shared_ptr<PipelineLibrary> pipelineLibrary){ this->pipelineLibrary = pipelineLibrary; return *this;}
                virtual Builder& WithResourceManager();         Builder& WithResourceManager(std::shared_ptr<ResourceManager> resourceManager){ this->resourceManager = resourceManager; return *this;}
                virtual Builder& WithMeshes();                  Builder& WithMeshes(std::unordered_map<std::string, Mesh*> meshes);
                virtual Builder& WithDescriptorSets();          Builder& WithDescriptorSets(std::vector<std::vector<BufferResource*>>& descriptorSets);
                virtual Builder& WithResourceMap();             Builder& WithResourceMap(std::unordered_map<std::string, BufferResource*> resourceMap);
                virtual Builder& WithTextures(std::unordered_set<std::string>);
                
                
                

            protected:
                std::shared_ptr<Shell> shell;
                std::shared_ptr<VulkanEngine> engine;
                std::shared_ptr<GPU> gpu;
                std::shared_ptr<SwapchainManager>swapchainManager;
                std::shared_ptr<DescriptorAllocator> descriptorAllocator;                                        
                std::shared_ptr<SyncManager> syncManager;
                std::shared_ptr<PipelineLibrary>pipelineLibrary;
                std::shared_ptr<RenderpassLibrary> renderpassLibrary;
                std::shared_ptr<FramebufferLibrary> framebufferLibrary;
                std::shared_ptr<CommandManager> commandManager;
                std::shared_ptr<ResourceManager> resourceManager;
                std::unordered_map<std::string, DescriptorFile> descriptorFiles;


                static std::vector<std::filesystem::path>  GetAllFiles(std::string repository);
                
            private:
                std::vector<const char*> deviceExtensions = {
                    VK_KHR_SWAPCHAIN_EXTENSION_NAME                  // Required for presentation
                    // VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,         // Enables render pass 2 structures
                    // VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME // Enables separate depth/stencil layouts
                };         
               
                const std::string RENDER_PASS_FILES_DIRECTORY = OUTPUT_DIRECTORY "/renderpasses"; //relative to build
                const std::string PIPELINE_FILES_DIRECTORY = OUTPUT_DIRECTORY "/pipelines";  //relative to build         
                const std::string DESCRIPTOR_FILES_DIRECTORY = OUTPUT_DIRECTORY "/descriptors";  //relative to build    
                const std::string TEXTURE_FILES_DIRECTORY = OUTPUT_DIRECTORY "/textures";  //relative to build
        };
    
    protected:
        std::shared_ptr<VulkanEngine> engine;
        std::shared_ptr<Shell> shell;
        std::shared_ptr<GPU> gpu;
        std::shared_ptr<SyncManager> syncManager;
        std::shared_ptr<SwapchainManager>swapchainManager;
        std::shared_ptr<PipelineLibrary>pipelineLibrary;
        std::shared_ptr<RenderpassLibrary> renderpassLibrary;
        std::shared_ptr<FramebufferLibrary> framebuffersLibrary;
        std::shared_ptr<CommandManager> commandManager;
        std::unordered_map<std::string, Shader> shaders;
        std::shared_ptr<DescriptorAllocator> descriptorAllocator; //static allocator
        std::vector<DescriptorAllocator> perFrameDescriptorAllocator;  //max frames size
        std::shared_ptr<Renderer> renderer;
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        std::shared_ptr<ResourceManager> resourceManager;
        std::unordered_map<std::string, DescriptorFile> descriptorFiles;
        CameraUBO sceneCamera; 
        CameraUBO UICamera;
        glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 5.0f);                  // Camera at z = 5
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.5f);            // Looking at center of square
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);           // Up direction
        
        
        virtual void CreateMoniliths();
        virtual void AllocateToMonoliths();
        virtual void AllocateMeshes();
        virtual void AllocateDescriptorResources();
        virtual void InitResources();
    private:
        uint32_t MAX_FRAMES;
        float angle = glm::pi<float>() * 1.0f;
        float cameraspeed = 0.003;
        float orbitalDistance = 5.0f;   
        float yaw = 0.0f; //horizontal angle in randians
        
};