#include "DescriptorsetLoader.hpp"
using json = nlohmann::json;

void DescriptorsetLoader::LoadFromFile(std::string& filePath,
                                       std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorSetsOut,
                                       std::unordered_map<uint32_t, std::vector<std::string>>& descriptorNamesOut){
    
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open graphics config: " + filePath);
    }
    json j;
    in >> j;
    
}