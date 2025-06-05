#include "SceneNode.hpp"

// void SceneNode::DrawGeometry(VkCommandBuffer cmdBuf, uint32_t currentFrame) {
//     auto pipeline = pipelineManager->GetPipeline();
//     if (mesh && pipeline) {
//         vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

//         if (descriptorSetIndex != static_cast<uint32_t>(-1) && descriptorAllocator) {
//             auto& sets = descriptorAllocator->GetDescriptorSets();
//             uint32_t numSetsPerFrame = descriptorAllocator->GetDescriptorSetLayouts().size();
//             VkDescriptorSet set = sets[currentFrame * numSetsPerFrame + descriptorSetIndex];

//             vkCmdBindDescriptorSets(cmdBuf,
//                 VK_PIPELINE_BIND_POINT_GRAPHICS,
//                 pipelineManager->GetPipelineLayout(),
//                 0, 1, &set,  0, nullptr); 

//         }

//         mesh->Bind(cmdBuf);
//         mesh->Draw(cmdBuf);
//     }

//     for (auto& child : children) {
//         child->DrawGeometry(cmdBuf, currentFrame);
//     }
// }

// void SceneNode::DrawUI(VkCommandBuffer cmdBuf, uint32_t currentFrame) {
//     auto pipeline = pipelineManager->GetPipeline();
//     if (mesh && pipeline) {
//         vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

//         if (descriptorSetIndex != static_cast<uint32_t>(-1) && descriptorAllocator) {
//             auto& sets = descriptorAllocator->GetDescriptorSets();
//             uint32_t numSetsPerFrame = descriptorAllocator->GetDescriptorSetLayouts().size();
//             VkDescriptorSet set = sets[currentFrame * numSetsPerFrame + descriptorSetIndex];

//             vkCmdBindDescriptorSets(cmdBuf,
//                 VK_PIPELINE_BIND_POINT_GRAPHICS,
//                 pipelineManager->GetPipelineLayout(),
//                 0, 1, &set,  0, nullptr); 

//         }

//         mesh->Bind(cmdBuf);
//         mesh->Draw(cmdBuf);
//     }
//     for (auto& child : children) {
//         child->DrawUI(cmdBuf, currentFrame);
//     }

   
// }