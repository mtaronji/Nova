
#pragma once
#include <vulkan/vulkan.h>
#include "json.hpp"
#include <vector>
#include <string>
#include <optional>
#include <fstream>

struct AttachmentDescriptionJson {
    VkFormat format;
    VkSampleCountFlagBits samples;
    VkAttachmentLoadOp loadOp;
    VkAttachmentStoreOp storeOp;
    VkAttachmentLoadOp stencilLoadOp;
    VkAttachmentStoreOp stencilStoreOp;
    VkImageLayout initialLayout;
    VkImageLayout finalLayout;
};

struct AttachmentReferenceJson {
    uint32_t attachment;
    VkImageLayout layout;
};

struct SubpassDescriptionJson {
    std::vector<AttachmentReferenceJson> colorAttachments;
    std::optional<AttachmentReferenceJson> depthStencilAttachment;
};

struct SubpassDependencyJson {
    uint32_t srcSubpass;
    uint32_t dstSubpass;
    VkPipelineStageFlags srcStageMask;
    VkPipelineStageFlags dstStageMask;
    VkAccessFlags srcAccessMask;
    VkAccessFlags dstAccessMask;
};

struct RenderPassJson {
    std::vector<AttachmentDescriptionJson> attachments;
    std::vector<SubpassDescriptionJson> subpasses;
    std::vector<SubpassDependencyJson> dependencies;
};

class RenderPassLoader {
public:
    static RenderPassJson LoadFromFile(const std::string& filePath);
};