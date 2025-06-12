#pragma once

#include "Vertex.hpp"

#include <vulkan/vulkan.h>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include "DescriptorsetLoader.hpp"

class PipelineManager;
class DescriptorAllocator;
class PipelineLibrary {
    public:
        PipelineLibrary() = default;
        PipelineLibrary(const PipelineLibrary&) = delete;
        PipelineLibrary& operator=(const PipelineLibrary&) = delete;
        void AddPipeline(std::string name, PipelineManager* pipeline);
    
        PipelineManager* GetPipeline(const std::string& name);
        std::unordered_map<std::string, PipelineManager*>& GetPipelines();

        std::vector<std::vector<VkDescriptorSetLayoutBinding>> GetAllDescriptorBindings(std::unordered_map<std::string, DescriptorFile>& descriptorFiles);
        virtual void CreatePipelines();  //if you add more vertex types this must get overriden. That's why virtual
        virtual void CreateDescriptorSetLayouts(std::shared_ptr<DescriptorAllocator> descriptorAllocator);
        void Cleanup();
    
    protected:
        std::unordered_map<std::string, PipelineManager*> pipelines;
        
};