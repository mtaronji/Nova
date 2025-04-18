#pragma once
#include "Shell.hpp"
#include "Renderer.hpp"
#include "VulkanEngine.hpp"
#include "GPU.hpp"
#include "PipelineConfig.hpp"
#include "Pipeline.hpp"
#include "RenderPass.hpp"
#include "VulkanResourceManager.hpp"
#include "CommandManager.hpp"
#include "FramebufferManager.hpp"
#include "Shader.hpp"
#include "SyncManager.hpp"


class Magusv0{
    public:
        Magusv0(const Renderer &renderer, const Shell &shell);
        void Start();


        class Builder{
            
        };
    
    private:
        VulkanEngine engine;
        
};