#pragma once

#include <vulkan/vulkan.h>

class GPU;

class RenderPass {
public:
    RenderPass(const GPU& gpu, VkFormat colorFormat);
    ~RenderPass();

    VkRenderPass Get() const { return renderPass; }

private:
    const GPU& gpu;
    VkRenderPass renderPass;

    void createRenderPass(VkFormat colorFormat);
};