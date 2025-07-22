#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <json.hpp>
#include <optional>

enum DesciptorUsageScope {
    USAGE_SCOPE_PER_FRAME,
    USAGE_SCOPE_PER_OBJECT,
    USAGE_SCOPE_SINGULAR,
    USAGE_SCOPE_GLOBAL
};
struct DescriptorSetInfo {
    std::vector<VkDescriptorSetLayoutBinding> descriptorBindings;
    DesciptorUsageScope scope = USAGE_SCOPE_SINGULAR;
    std::unordered_map<VkDescriptorType, uint32_t> descriptorCounts;
};

struct DescriptorFile {
    std::vector<DescriptorSetInfo> descriptorSetsInfo;
    std::vector<std::vector<VkDescriptorSetLayoutBinding>> setsBindings;
    std::vector<std::vector<std::string>> descriptorNames; //per set per binding
    std::string fileName;
    std::vector<std::vector<std::optional<uint32_t>>> requestedLayers;           //only for use in images thus optional
    std::vector < std::vector<std::optional<uint32_t>>> requestedMipLevels;      //only for use in images thus optional
    std::vector<std::vector<VkDescriptorSetLayoutBinding>>& GetDescriptorBindings() {
        return setsBindings;
    }
};
class DescriptorsetLoader {
    public:    
        static DescriptorFile LoadFromFile(std::string filePath);
  

    protected:
        DescriptorsetLoader() = default;                                                 
};