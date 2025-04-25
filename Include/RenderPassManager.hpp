#pragma once

#include <vulkan/vulkan.h>
#include "GPU.hpp"
#include <vector>
#include <memory>

struct AttachmentInfo {
    VkAttachmentDescription description;
    enum class Type { Color, Depth, Input, Resolve } type;
    std::string name;
};

class RenderPassManager {
    public:
        RenderPassManager(std::shared_ptr<GPU> gpu, const VkSubpassDescription& subpassDescription, const std::vector<VkAttachmentDescription>& attachmentDescriptions);
        ~RenderPassManager();

        void NotifyRenderPassOutOfDate();
        VkRenderPass GetRenderPass() const { return renderPass; }

        class Builder{
            public:
                Builder() = default;

                Builder& WithGPU(std::shared_ptr<GPU> gpu) {
                    this->gpu = gpu;
                    return *this;
                }
                Builder& WithAttachments(const std::vector<AttachmentInfo>& attachmentInfos, std::vector<uint32_t>& preserveAttachments);

                std::shared_ptr<RenderPassManager> Build();

            private:
                std::shared_ptr<GPU> gpu;
                std::vector<VkAttachmentReference> colorAttachments;
                std::vector<VkAttachmentReference> inputAttachments;
                std::vector<VkAttachmentReference> resolveAttachments;
                std::optional<VkAttachmentReference> depthStencilAttachment;
                std::vector<uint32_t> preserveAttachments;
                VkFormat colorFormat;
                std::vector<VkAttachmentReference> attachmentrefs;
                std::vector<VkAttachmentDescription> attachmentDescriptions;


        };

    protected:
        VkRenderPass renderPass;
        std::shared_ptr<GPU> gpu;
        VkSubpassDescription subpassDescription;
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        void CreateRenderPass();
};
