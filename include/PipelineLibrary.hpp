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

        virtual void CreatePipelines();  //if you add more vertex types this must get overriden. That's why virtual
        virtual void CreateDescriptorSetLayouts(std::shared_ptr<DescriptorAllocator> descriptorAllocator);

        void Cleanup();
    
    protected:
        std::unordered_map<std::string, PipelineManager*> pipelines;
        
};