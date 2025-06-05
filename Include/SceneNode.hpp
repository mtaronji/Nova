#pragma once

// #include "DescriptorAllocator.hpp"
// #include "PipelineManager.hpp"

// #include <vector>
// #include <memory>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <vulkan/vulkan.h>

//     class Mesh;
//     class DescriptorAllocator;
//     class PipelineManager;

//     class SceneNode {
//         public:
//             SceneNode();
//             virtual ~SceneNode() = default;

//             void AddChild(std::shared_ptr<SceneNode> child);
//             void SetParent(SceneNode* parent);

//             void SetLocalTransform(const glm::mat4& transform);
//             const glm::mat4& GetWorldTransform() const;

//             void Update(const glm::mat4& parentTransform);
//             void DrawGeometry(VkCommandBuffer cmdBuf, uint32_t currentFrame); // needs frame index
//             void DrawUI(VkCommandBuffer cmdBuf, uint32_t currentFrame);

//             void SetMesh(Mesh* m) { mesh = m; }
//             void SetPipelineManager(PipelineManager* p) { pipelineManager = p; }

//             void SetDescriptorSetIndex(uint32_t setIndex) { descriptorSetIndex = setIndex; }
//             void SetDescriptorAllocator(DescriptorAllocator *d){descriptorAllocator = d;}

//         protected:
//             glm::mat4 localTransform = glm::mat4(1.0f);
//             glm::mat4 worldTransform = glm::mat4(1.0f);         
//             SceneNode* parent = nullptr;
//             std::vector<std::shared_ptr<SceneNode>> children;

//             Mesh* mesh = nullptr;
//             PipelineManager* pipelineManager = nullptr;
//             DescriptorAllocator* descriptorAllocator;
//             // Index into DescriptorAllocator descriptorSets
//             uint32_t descriptorSetIndex = -1;

//         private:
    
//     };