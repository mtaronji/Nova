#pragma once

#include "RenderPass.hpp"
#include "Pipeline.hpp"
#include "GPU.hpp"
#include "SwapchainManager.hpp"

class Renderer {
public:
    Renderer(const GPU& gpu, const SwapchainManager& swapchain, VkRenderPass renderPass, VkDescriptorSetLayout layout);
    ~Renderer();

    void DrawFrame();

private:
    const GPU& gpu;
    const SwapchainManager& swapchain;

    Pipeline pipeline;
    VkRenderPass renderPass;
};