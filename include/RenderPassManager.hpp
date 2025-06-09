#pragma once

#include <vulkan/vulkan.h>
#include "GPU.hpp"
#include <vector>
#include <memory>
#include <cassert>
#include <string>
#include "RenderpassLoader.hpp"


class RenderPassManager {
    public:
        static RenderPassManager* Create(std::shared_ptr<GPU> gpu,std::string file);
       
        RenderPassManager() = delete;
        ~RenderPassManager();

        void NotifyRenderPassOutOfDate();
        VkRenderPass GetRenderPass() const { return renderPass; } 
        VkRenderPassCreateInfo LoadRenderpassConfig(const std::string path);

        std::vector<VkAttachmentDescription>& GetAttachmentDescriptions(){return attachmentDescriptions;}
        std::vector<VkImageUsageFlags> & GetImageUseFlags(){return imageUsesForAttachments;}
        std::vector<VkImageAspectFlags> & GetImageAspectFlags(){return aspectFlagsForAttachments;}
        std::vector<VkClearValue>& GetClearValues() { return clearValues; }
        // std::vector<VkAttachmentDescription>& GetResolveAttachmentDescriptions(){return resolveAttachmentDescriptions;}
        // std::vector<VkAttachmentDescription>& GetInputAttachmentDescriptions(){return inputAttachmentDescriptions;}
        // std::vector<VkAttachmentDescription>& GetStencilAttachmentDescriptions(){return stencilAttachmentDescriptions;}
        // std::vector<VkAttachmentDescription>& GetDepthAttachmentDescriptions(){return depthAttachmentDescriptions;}
        // std::vector<VkAttachmentDescription>& GetColorAttachmentDescriptions(){return colorAttachmentDescriptions;}

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
        RenderPassManager(std::shared_ptr<GPU> gpu,std::string file);
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        std::vector<VkAttachmentDescription> depthAttachmentDescriptions;
        std::vector<VkAttachmentDescription> resolveAttachmentDescriptions;
        std::vector<VkAttachmentDescription> colorAttachmentDescriptions;
        std::vector<VkAttachmentDescription> stencilAttachmentDescriptions;
        std::vector<VkAttachmentDescription> inputAttachmentDescriptions;
        std::vector<VkSubpassDescription> subpassDescriptions;
        std::vector<VkSubpassDependency> subpassdependencies;
        std::vector<VkAttachmentReference> colorAtachmentRefs;
        std::vector<VkAttachmentReference> depthAtachmentRefs;
        std::vector<VkAttachmentReference> resolveAtachmentRefs;
        std::vector<VkImageUsageFlags> imageUsesForAttachments;
        std::vector<VkImageAspectFlags> aspectFlagsForAttachments;
        std::vector<VkClearValue> clearValues;
        VkRenderPass renderPass;
        std::shared_ptr<GPU> gpu;
        void CreateRenderPass();
        void SetPresentColorAttachmentFormat();
        VkRenderPassCreateInfo info;
};
