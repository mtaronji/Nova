#pragma once

#include <vulkan/vulkan.h>
#include "GPU.hpp"
#include <vector>
#include <memory>
#include <cassert>
#include "RenderpassLoader.hpp"


class RenderPassManager {
    public:
        RenderPassManager(std::shared_ptr<GPU> gpu);
        RenderPassManager() = delete;
        ~RenderPassManager();

        void NotifyRenderPassOutOfDate();
        VkRenderPass GetRenderPass() const { return renderPass; } 
        VkRenderPassCreateInfo LoadRenderpassConfig(const std::string path);

        void UpdateColorAttachmentFormats(
            VkFormat oldColorFormat,
            VkFormat newColorFormat
        );

        void UpdateDepthAttachmentFormats(
            VkFormat oldDepthFormat,
            VkFormat newDepthFormat
        );

        void Cleanup();
        

    protected:
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        std::vector<VkSubpassDescription> subpassDescriptions;
        std::vector<VkSubpassDependency> subpassdependencies;
        std::vector<VkAttachmentReference> colorAtachmentRefs;
        std::vector<VkAttachmentReference> depthAtachmentRefs;
        VkRenderPass renderPass;
        std::shared_ptr<GPU> gpu;
        void CreateRenderPass();
        void SetPresentColorAttachmentFormat();
        VkRenderPassCreateInfo info;
};
