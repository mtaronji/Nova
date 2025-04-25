#include "GraphicsPipelineLoader.hpp"

using json = nlohmann::json;

VkGraphicsPipelineCreateInfo GraphicsPipelineLoader::LoadFromFile(std::shared_ptr<GPU> gpu, const std::string& filePath){
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open render pass file: " + filePath);
    }
    json j;
    in >> j;
    
    for (const auto& a : j["shaderStages"]) {
        
        if(a["stage"].is_string() && a["stage"] == "vertex" && a["path"].is_string()){

            Shader vertexShader = Shader(gpu, static_cast<std::string>(a["path"]), VK_SHADER_STAGE_VERTEX_BIT);
        }
        if(a["stage"].is_string() && a["stage"] == "fragment"){
            Shader fragmentShader = Shader(gpu, static_cast<std::string>(a["path"]), VK_SHADER_STAGE_FRAGMENT_BIT);
        }
        if(a["stage"].is_string() && a["stage"] == "geometry"){
            Shader geometryShader = Shader(gpu, static_cast<std::string>(a["path"]), VK_SHADER_STAGE_GEOMETRY_BIT);
        }
        if(a["stage"].is_string() && a["stage"] == "tessellationControlShader"){
            Shader tessellationControlShaderShader = Shader(gpu, static_cast<std::string>(a["path"]), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
        }
        if(a["stage"].is_string() && a["stage"] == "tessellationEvaluationShader"){
            Shader tessellationEvaluationShaderShader = Shader(gpu, static_cast<std::string>(a["path"]), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
        }
        if(a["stage"].is_string() && a["stage"] == "computeShader"){
            Shader computeShaderShader = Shader(gpu, static_cast<std::string>(a["path"]), VK_SHADER_STAGE_COMPUTE_BIT);
        }

        
    }
    for (const auto& a : j["inputAssembly"]) {
        
    }
    for (const auto& a : j["viewport"]) {
        
    }
    for (const auto& a : j["scissor"]) {
        
    }
    for (const auto& a : j["rasterizer"]) {
        
    }
    for (const auto& a : j["multisampling"]) {
        
    }
    for (const auto& a : j["depthStencil"]) {
        
    }
    for (const auto& a : j["colorBlend"]) {
        
    }
  
    VkGraphicsPipelineCreateInfo createInfo = {};

    return createInfo;
    
}

