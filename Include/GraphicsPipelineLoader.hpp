// GraphicsPipelineLoader.hpp
#pragma once

#include <vulkan/vulkan.h>
#include "json.hpp"
#include <vector>
#include <string>
#include <optional>
#include <fstream>
#include "json.hpp"
#include "Shader.hpp"
#include "GPU.hpp"
#include <memory>
#include "Mesh.hpp"
#include <unordered_set>

class ResourceManager;
class GraphicsPipelineLoader {
    public:
        GraphicsPipelineLoader() = default;
        static void LoadFromFile(
                                const std::string& filePath,
                                std::vector<char> & vertexShaderCodeOut,
                                std::vector<char> & fragmentShaderCodeOut,
                                std::vector<char> & computeShaderCodeOut,
                                std::vector<char> & geometryShaderCodeOut,
                                VkPipelineInputAssemblyStateCreateInfo& inputAssemblyOut,
                                VkPipelineRasterizationStateCreateInfo& rasterizerCreateInfoOut,
                                VkPipelineDepthStencilStateCreateInfo& depthStencilOut,
                                VkPipelineMultisampleStateCreateInfo& multisamplingOut,
                                VkPipelineColorBlendStateCreateInfo& colorBlendingOut,
                                std::vector<VkPipelineColorBlendAttachmentState>& colorblendAttachmentsOut,
                                std::vector<VkDynamicState>& dynamicStatesOut,
                                std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorSetsOut,
                                std::vector<std::string> &descriptorNames,
                                std::vector<VkPushConstantRange>& pushConstantRangesOut
                                );

    protected:
                                                        
  

};
