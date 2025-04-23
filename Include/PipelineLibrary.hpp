#pragma once

#include "PipelineManager.hpp"
#include <unordered_map>
#include <memory>

class PipelineLibrary {
    public:
        void AddPipeline(std::string name, std::unique_ptr<PipelineManager> pipeline) {
            pipelines[name] = std::move(pipeline);
        }
    
        PipelineManager* GetPipeline(const std::string& name) {
            return pipelines.at(name).get();
        }
    
    protected:
        std::unordered_map<std::string, std::unique_ptr<PipelineManager>> pipelines;
    };