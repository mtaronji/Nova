#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

class FramebufferManager {
public:
    FramebufferManager() = default;

    FramebufferManager(const VkDevice &device,
                const VkRenderPass &renderPass,
                const std::vector<VkImageView>& imageViews,
                const VkExtent2D &extent);

    const std::vector<VkFramebuffer>& GetFramebuffers() const;
    

    void Cleanup(const VkDevice& device);

    class Builder{
        public:
            Builder& SetGPU(const VkDevice* device){
                this->device = device;
                return *this;
            }
            Builder& SetRenderPass(const VkRenderPass* renderPass){
                this->renderPass = renderPass;
                return *this;
            }
            Builder& SetImageViews(const std::vector<VkImageView>* imageViews){
                this->imageViews = imageViews;
                return *this;
            }
            Builder& SetExtent(const VkExtent2D* extent){
                this->extent = extent;
                return *this;
            }
            FramebufferManager Build(){
                return FramebufferManager(*device, *renderPass, *imageViews, *extent);
            }

        private:
            const VkDevice* device;
            const VkRenderPass* renderPass;
            const std::vector<VkImageView>* imageViews;
            const VkExtent2D* extent;
    };

private:
    std::vector<VkFramebuffer> framebuffers;
    const VkDevice &device;
    const VkRenderPass &renderPass;
    const std::vector<VkImageView> & imageViews;
    const VkExtent2D &extent;
};