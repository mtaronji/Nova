#pragma once


#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include "PushConstants.hpp"
#include <string>

class Shell;
class GPU;
class VulkanEngine;
class SwapchainManager;
class SyncManager;
class PipelineLibrary;
class RenderpassLibrary;
class CommandManager;
class FramebufferLibrary;
class DescriptorAllocator;
class ResourceManager;
class RenderPassManager;
class FramebufferGenerator;
class PipelineManager;

class Renderer {
    public:
        Renderer(   
            std::shared_ptr<Shell> shell,
            std::shared_ptr<GPU> gpu,
            std::shared_ptr<VulkanEngine> engine,
            std::shared_ptr<SwapchainManager> swapchainmanager,
            std::shared_ptr<SyncManager> syncmanager,
            std::shared_ptr<PipelineLibrary> pipelineLibrary,
            std::shared_ptr<RenderpassLibrary> renderpassLibrary,
            std::shared_ptr<CommandManager> commandmanager,
            std::shared_ptr<FramebufferLibrary> framebuffersLibrary,
            std::shared_ptr<DescriptorAllocator> descriptorAllocator,
            std::shared_ptr<ResourceManager> resourceManager
        );
        Renderer() = delete;
        ~Renderer();

        void virtual DrawFrame();

        void virtual SetFrameTime(float deltaTime);
        
        void NotifySwapchainOutOfDate(); 

        virtual void BindPipeline(std::string pipelineKey);

        std::vector<std::vector<VkDescriptorSet>>& GetPipelineDescriptorSets(std::string pipelineKey);
        VkDescriptorSet& GetPipelineDescriptorSet(std::string pipelineKey, uint32_t frame, uint32_t set); //the descriptorset for the set index and frame
        std::vector<VkDescriptorSet>& GetPipelineDescriptorSetsFrame(std::string pipelineKey, uint32_t frame); //the descriptorset for the frame

        uint32_t GetFrameIndex() const {return currentFrame;}

        void AllocateDescriptorSets(std::string pipelineKey);

        void Cleanup();
        
    protected:
        void virtual DrawFrameCommands(VkCommandBuffer commandBuffer, 
                                        uint32_t imageIndex
                                        );

        std::shared_ptr<GPU> gpu;
        std::shared_ptr<VulkanEngine> engine;
        std::shared_ptr<SwapchainManager> swapchainmanager;
        std::shared_ptr<SyncManager> syncmanager;
        std::shared_ptr<PipelineLibrary> pipelineLibrary;
        std::shared_ptr<RenderpassLibrary> renderpassLibrary;
        std::shared_ptr<CommandManager> commandmanager;
        std::shared_ptr<FramebufferLibrary> framebuffersLibrary;
        std::shared_ptr<Shell> shell;
        std::shared_ptr<DescriptorAllocator> descriptorAllocator;
        std::shared_ptr<ResourceManager> resourceManager;
        std::vector<VkFence> imagesInFlight; // size = swapchain image count

        std::unordered_map<std::string, std::vector<std::vector<VkDescriptorSet>>> pipelineDescriptorSets;  ////[frame][set] //we set it like this when we allocate
   
        //the following 3 variables are initially null. Must bind a pipeline to set them
        RenderPassManager* renderpassmanager = nullptr;
        PipelineManager* pipelineManager = nullptr;
        FramebufferGenerator* framebufferContainer = nullptr;

        uint32_t currentFrame = 0;
        uint32_t  MAX_FRAMES;
        FrameInfo frameinfo;
        std::string currentPipelineKey;
        bool framebufferResized = false;

        void RecreateSwapchain();
        void DestroyFrameBuffers();
        void CreateFrameBuffers();


};