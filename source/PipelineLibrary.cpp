
#include "PipelineLibrary.hpp"
#include "PipelineManager.hpp"
#include "DescriptorAllocator.hpp"


void PipelineLibrary::CreatePipelines(){

    for(auto& [key,val] : pipelines){
        std::string vtype = val->GetVertexType();

        if(vtype == "p"){val->CreateGraphicsPipeline<VertexP>();}
        else if(vtype == "pc"){val->CreateGraphicsPipeline<VertexPC>();}
        else if(vtype == "pn"){val->CreateGraphicsPipeline<VertexPN>();}
        else if(vtype == "pnt"){val->CreateGraphicsPipeline<VertexPNT>();}
        else if(vtype == "pt"){val->CreateGraphicsPipeline<VertexPT>();}
        else if(vtype == "pntc"){val->CreateGraphicsPipeline<VertexPNTC>();}
        else{
            throw std::runtime_error("Cannot create graphics pipeline from vertex type. Not supported");
        }

    }
}

 std::unordered_map<std::string, PipelineManager*>& PipelineLibrary::GetPipelines() {
    return pipelines;
}

PipelineManager* PipelineLibrary::GetPipeline(const std::string& name) {
    return pipelines.at(name);
}

void PipelineLibrary::CreateDescriptorSetLayouts(std::shared_ptr<DescriptorAllocator> descriptorAllocator){
     for(auto& [name, pipelineManager] : pipelines){
        descriptorAllocator->CreateDescriptorSetLayout(pipelineManager->GetDescriptorSetBindings(), pipelineManager->GetDescriptorSetLayouts());
     }
}

void PipelineLibrary::AddPipeline(std::string name, PipelineManager* pipeline) {
    pipelines[name] = pipeline;  
    auto& bindings = pipeline->GetDescriptorSetBindings();   
}

void PipelineLibrary::Cleanup(){
    for(auto& [name, pipeline] : pipelines){
        pipeline->Cleanup();
        delete(pipeline);
    }
}