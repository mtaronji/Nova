#pragma once

#include <vulkan/vulkan.h>
#include "json.hpp"
#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <fstream>

class RenderPassLoader {
public:
    static  VkRenderPassCreateInfo LoadFromFile(const std::string& filePath,
                                                std::vector<VkAttachmentDescription>& attachmentDescriptionsOut,
                                                std::vector<VkSubpassDescription>& subpassDescriptionsOut,
                                                std::vector<VkSubpassDependency>& subpassdependenciesOut,
                                                std::vector<VkAttachmentReference>& colorAtachmentRefsOut,
                                                std::vector<VkAttachmentReference>& depthAtachmentRefsOut,
                                                std::vector<VkAttachmentReference>& resolveAtachmentRefsOut,
                                                std::vector<VkImageUsageFlags>& imageUsesForAttachmentsOut,
                                                std::vector<VkImageAspectFlags>& aspectFlagsForAttachmentsOut);
};
