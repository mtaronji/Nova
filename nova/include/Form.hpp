#pragma once

 #include "DescriptorAllocator.hpp"
 #include "PipelineManager.hpp"

 #include <vector>
 #include <memory>
 #include <glm/glm.hpp>
 #include <glm/gtc/matrix_transform.hpp>
 #include <vulkan/vulkan.h>

class Mesh;
class DescriptorAllocator;
class PipelineManager;

class Form {
    public:
        Form() = default;
        Form(std::shared_ptr<Form> parent) { this->parent = parent; }
        virtual ~Form() = default;

        void AddChild(std::shared_ptr<Form> child) { this->children.push_back(child); }
        void SetParent(std::shared_ptr<Form> parent) { this->parent = parent; }

        void SetTransform(const glm::mat4& transform) { this->transform = transform; }
        const glm::mat4& GetTransform() const { return transform; }

        void DrawGeometry(VkCommandBuffer cmdBuf, uint32_t currentFrame); // needs frame index
        void DrawUI(VkCommandBuffer cmdBuf, uint32_t currentFrame);
        
        void SetMesh(std::shared_ptr<Mesh> m) { mesh = m; }
        void SetPipelineManager(std::shared_ptr<PipelineManager> p) { pipelineManager = p; }

        void SetDescriptorAllocator(std::shared_ptr<DescriptorAllocator> d){descriptorAllocator = d;}

        VkCommandBuffer& GPUCommands(VkCommandBuffer cmds, uint32_t currentFrame);

    protected:
 
        glm::mat4 transform = glm::mat4(1.0f);         
        std::shared_ptr<Form> parent = nullptr;
        std::vector<std::shared_ptr<Form>> children;

        std::shared_ptr<Mesh> mesh = nullptr;
        std::shared_ptr<PipelineManager> pipelineManager = nullptr;
        std::shared_ptr<DescriptorAllocator> descriptorAllocator;


    private:
    
};