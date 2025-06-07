#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_set>
#include "json.hpp"

class DescriptorsetLoader {
    public:    
        static void LoadFromFile(std::string& filePath,
                                std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorSetsOut,
                                std::unordered_map<uint32_t, std::vector<std::string>>& descriptorNamesOut
                                );

    protected:
        DescriptorsetLoader() = default;                                                 
  

};