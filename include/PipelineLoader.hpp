// GraphicsPipelineLoader.hpp
#pragma once
#include "json.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <optional>
#include <fstream>


#include <memory>
#include <unordered_set>

class ResourceManager;
class PipelineLoader {
    public:
        PipelineLoader() = default;
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
                                std::unordered_map<uint32_t, std::vector<std::string>>& descriptorNamesOut,
                                std::vector<VkPushConstantRange>& pushConstantRangesOut,
                                std::string& renderpassKeyOut,
                                std::string& vertexTypeOut,
                                std::string& descriptorFile
                                );

    protected:
                                                        
  

};
