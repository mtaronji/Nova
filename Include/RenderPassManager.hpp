#pragma once

#include <vulkan/vulkan.h>
#include "GPU.hpp"
#include <vector>

struct AttachmentInfo {
    VkAttachmentDescription description;
    enum class Type { Color, Depth, Input, Resolve } type;
    std::string name;
};

class RenderPassManager {
    public:
        RenderPassManager(const GPU& gpu, const VkSubpassDescription& subpassDescription, const std::vector<VkAttachmentDescription>& attachmentDescriptions);
        ~RenderPassManager();

        void NotifyRenderPassOutOfDate();
        VkRenderPass Get() const { return renderPass; }

        class Builder{
            Builder() = delete;

            Builder& SetGPU(const GPU* gpu) {
                this->gpu = gpu;
                return *this;
            }
            Builder& SetAttachments(const std::vector<AttachmentInfo>& attachmentInfos, const std::vector<uint32_t>& preserveAttachments);

            RenderPassManager Build();

            private:
                const GPU* gpu;
                std::vector<VkAttachmentReference> colorAttachments;
                std::vector<VkAttachmentReference> inputAttachments;
                std::vector<VkAttachmentReference> resolveAttachments;
                std::optional<VkAttachmentReference> depthStencilAttachment;
                std::vector<uint32_t> preserveAttachments;
                const VkFormat* colorFormat;
                std::vector<VkAttachmentReference> attachmentrefs;
                std::vector<VkAttachmentDescription> attachmentDescriptions;


        };

    private:
        VkRenderPass renderPass;
        const GPU& gpu;
        const VkSubpassDescription& subpassDescription;
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        void CreateRenderPass();
};
