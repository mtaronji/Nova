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
        VkRenderPass GetRenderPass() const { return renderPass; }

        class Builder{
            public:
                Builder() = default;

                Builder& SetGPU(const GPU* gpu) {
                    this->gpu = gpu;
                    return *this;
                }
                Builder& SetAttachments(const std::vector<AttachmentInfo>& attachmentInfos, std::vector<uint32_t>* preserveAttachments = nullptr );

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

    protected:
        VkRenderPass renderPass;
        const GPU& gpu;
        const VkSubpassDescription& subpassDescription;
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        void CreateRenderPass();
};
