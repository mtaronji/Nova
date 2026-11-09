#pragma once


#include <vulkan/vulkan.h>
#include <json.hpp>
#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <fstream>


class RenderPassLoader {
    public:
     
        static  void LoadFromFile(const std::string& filePath,
            std::vector< VkRenderingAttachmentInfo>& colorAttachmentsOut,
            std::vector< VkRenderingAttachmentInfo>& depthAttachmentsOut,
            std::vector< VkRenderingAttachmentInfo>& stencilAttachmentsOut,
            std::vector< VkRenderingAttachmentInfo>& depthStencilAttachmentsOut,
            std::vector< VkRenderingAttachmentInfo>& resolveAttachmentsOut,
            std::vector< VkRenderingAttachmentInfo>& inputAttachmentsOut,
            std::vector<VkAttachmentDescription>& attachmentDescriptionsOut,
            std::vector<VkAttachmentReference>& attachmentReferencesOut,
            std::vector<VkAttachmentReference>& colorAtachmentRefsOut,
            std::vector<VkAttachmentReference>& depthAtachmentRefsOut,
            std::vector<VkAttachmentReference>& resolveAtachmentRefsOut,
            std::vector<VkAttachmentReference>& depthStencilAtachmentRefsOut,
            std::vector<VkAttachmentReference>& stencilAtachmentRefsOut,
            std::vector<VkAttachmentReference>& inputAtachmentRefsOut,
            std::vector<VkImageUsageFlags>& imageUsesForAttachmentsOut,
            std::vector<VkImageAspectFlags>& aspectFlagsForAttachmentsOut,
            std::vector<VkClearValue>& clearValuesOut,
			std::vector<uint32_t>& resolveAttachmentIndices,
            uint32_t & swapchainIndex);

        protected:
            RenderPassLoader() = default;
                                
};
