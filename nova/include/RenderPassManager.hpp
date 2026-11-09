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
        static RenderPassManager Create(std::shared_ptr<GPU> gpu,std::string file);
       
        RenderPassManager() = delete;

        ~RenderPassManager();

       
        void LoadRenderpassConfig(const std::string path);


        const std::vector<VkAttachmentDescription>& GetAttachmentDescriptions() const {return attachmentDescriptions;}
		const std::vector<VkAttachmentReference> GetAttachmentReferences() const { return attachmentRefs; }
        const std::vector<VkImageUsageFlags> & GetImageUseFlags() const {return imageUsesForAttachments;}
        const std::vector<VkImageAspectFlags> & GetImageAspectFlags() const {return aspectFlagsForAttachments;}
        const std::vector<VkClearValue>& GetClearValues()  const { return clearValues; }

        const std::vector<VkAttachmentDescription>& GetResolveAttachmentDescriptions() const {return resolveAttachmentDescriptions;}
        const std::vector<VkAttachmentDescription>& GetInputAttachmentDescriptions() const {return inputAttachmentDescriptions;}
        const std::vector<VkAttachmentDescription>& GetStencilAttachmentDescriptions() const {return stencilAttachmentDescriptions;}
        const std::vector<VkAttachmentDescription>& GetDepthAttachmentDescriptions() const {return depthAttachmentDescriptions;}
        const std::vector<VkAttachmentDescription>& GetColorAttachmentDescriptions() const {return colorAttachmentDescriptions;}
        const std::vector<VkAttachmentDescription>& GetDepthStencilAttachmentDescriptions() const { return depthStencilAttachmentDescriptions; }

        const std::vector<VkAttachmentReference>& GetResolveAttachmentRefs()  const { return resolveAtachmentRefs; }
        const std::vector<VkAttachmentReference>& GetInputAttachmentRefs() const { return inputAtachmentRefs; }
        const std::vector<VkAttachmentReference>& GetStencilAttachmentRefs() const { return stencilAtachmentRefs; }
        const std::vector<VkAttachmentReference>& GetDepthAttachmentRefs() const { return depthAtachmentRefs; }
        const std::vector<VkAttachmentReference>& GetColorAttachmentRefs()  const { return colorAtachmentRefs; }
        const std::vector<VkAttachmentReference>& GetDepthStencilAttachmentRefs() const { return depthStencilAtachmentRefs; }

        const std::vector<VkFormat>& GetResolveAttachmentFormats() const { return resolveAttachmentFormats; }
        const std::vector<VkFormat>& GetInputAttachmentFormats() const { return inputAttachmentFormats; }
        const std::vector<VkFormat> GetStencilAttachmentFormats() const { 
            
            if (!depthStencilAttachmentFormats.empty()) { return depthStencilAttachmentFormats; }
            if (!depthAttachmentFormats.empty()) { return depthAttachmentFormats; }

            return std::vector<VkFormat>  {VK_FORMAT_UNDEFINED };     
                  
        }
        const std::vector<VkFormat> GetDepthAttachmentFormats() const {

            if (!depthStencilAttachmentFormats.empty()) { return depthStencilAttachmentFormats; }
            if (!stencilAttachmentFormats.empty()) { return stencilAttachmentFormats; }

            return std::vector<VkFormat>  {VK_FORMAT_UNDEFINED };
        }
        const std::vector<VkFormat>& GetColorAttachmentFormats() const { return colorAttachmentFormats; }
        const std::vector<VkFormat>& GetDepthStencilAttachmentFormats() const { return depthStencilAttachmentFormats; }

        std::vector<VkRenderingAttachmentInfo>& GetColorRenderingAttachments()  { return colorAttachments; }
        std::vector<VkRenderingAttachmentInfo>& GetDepthRenderingAttachments() { return depthAttachments; }
        std::vector<VkRenderingAttachmentInfo>& GetStencilRenderingAttachments() { return stencilAttachments; }
        std::vector<VkRenderingAttachmentInfo>& GetDepthStencilRenderingAttachments() { return depthStencilAttachments; }
        std::vector<VkRenderingAttachmentInfo>& GetResolveRenderingAttachments() { return resolveAttachments; }
        std::vector<VkRenderingAttachmentInfo>& GetInputRenderingAttachments() { return inputAttachments; }

        void SetAttachmentFormats();
     

        const size_t GetColorAttachmentsCount() const {return colorAttachmentDescriptions.size();}
		const size_t GetDepthAttachmentsCount() const { return depthAttachmentDescriptions.size(); }
        const size_t GetStencilAttachmentsCount() const { return stencilAttachmentDescriptions.size(); }
		const size_t GetResolveAttachmentsCount()  const { return resolveAttachmentDescriptions.size(); }
        const size_t GetInputAttachmentsCount()  const { return inputAttachmentDescriptions.size(); }
        const uint32_t GetdepthStencilAttachmentsCount() const { return depthStencilAttachmentDescriptions.size(); }

        const uint32_t GetSwapchainAttachmentIndex() const { return swapchainAttachmentIndex; }
        const std::vector<uint32_t> GetResolveAttachmentIndices() const { return resolveAttachmentIndices; }
        VkRenderingInfo GetRenderingInfo() const { return renderingInfo; }
        
        void Cleanup();
        

    protected:
        RenderPassManager(std::shared_ptr<GPU> gpu,std::string file);
        std::vector<VkRenderingAttachmentInfo> renderingAttachments;
        std::vector< VkRenderingAttachmentInfo> colorAttachments;
        std::vector< VkRenderingAttachmentInfo> depthAttachments;
        std::vector< VkRenderingAttachmentInfo> stencilAttachments;
        std::vector< VkRenderingAttachmentInfo> depthStencilAttachments;
        std::vector< VkRenderingAttachmentInfo> resolveAttachments;
        std::vector< VkRenderingAttachmentInfo> inputAttachments;


        std::vector<VkAttachmentDescription> attachmentDescriptions;
        std::vector<VkAttachmentDescription> depthAttachmentDescriptions;
        std::vector<VkAttachmentDescription> resolveAttachmentDescriptions;
        std::vector<VkAttachmentDescription> colorAttachmentDescriptions;
        std::vector<VkAttachmentDescription> stencilAttachmentDescriptions;
        std::vector<VkAttachmentDescription> inputAttachmentDescriptions;
        std::vector<VkAttachmentDescription> depthStencilAttachmentDescriptions;

        std::vector<VkAttachmentReference> attachmentRefs;  
        std::vector<VkAttachmentReference> colorAtachmentRefs;
        std::vector<VkAttachmentReference> depthAtachmentRefs;
        std::vector<VkAttachmentReference> stencilAtachmentRefs;
        std::vector<VkAttachmentReference> resolveAtachmentRefs;
        std::vector<VkAttachmentReference> depthStencilAtachmentRefs;
        std::vector<VkAttachmentReference> inputAtachmentRefs;
        std::vector<VkImageUsageFlags> imageUsesForAttachments;
        std::vector<VkImageAspectFlags> aspectFlagsForAttachments;
        std::vector<VkClearValue> clearValues;
		std::vector<VkFormat> colorAttachmentFormats;
        std::vector<VkFormat> resolveAttachmentFormats;
        std::vector<VkFormat> depthAttachmentFormats;
        std::vector<VkFormat> stencilAttachmentFormats;
        std::vector<VkFormat> depthStencilAttachmentFormats;
        std::vector<VkFormat> inputAttachmentFormats;
        
        std::shared_ptr<GPU> gpu;
        uint32_t swapchainAttachmentIndex;
        std::vector<uint32_t> resolveAttachmentIndices;
        VkRenderingInfo renderingInfo;
 
        void SetPresentColorAttachmentFormat();
      
};
