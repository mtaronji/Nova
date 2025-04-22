#include "Shader.hpp"
#include "GPU.hpp"
#include <fstream>


Shader::Shader(GPU& gpu, const std::string& filepath, VkShaderStageFlagBits shaderStage) : gpu(gpu), shaderStage(shaderStage)
{

    auto code = ReadFile(filepath);  
    CreateShaderModule(code);
    CreateShaderStageInfo();
}


Shader::~Shader() {
    if (shaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(gpu.GetVkDevice(), shaderModule, nullptr);
    }
}


std::vector<char> Shader::ReadFile(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open shader file: " + path);

    size_t size = (size_t)file.tellg();
    std::vector<char> buffer(size);
    file.seekg(0);
    file.read(buffer.data(), size);
    return buffer;
}

void Shader::CreateShaderModule(const std::vector<char>& code) {

    VkShaderModuleCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = code.size();
    info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(gpu.GetVkDevice(), &info, nullptr, &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module.");
}

VkPipelineShaderStageCreateInfo Shader:: GetPipelineShaderStageCreateInfo() const {
    return this->pipelineShaderStageInfo;
}


void Shader::CreateShaderStageInfo(){
    
    pipelineShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageInfo.stage = shaderStage;
    pipelineShaderStageInfo.module = GetModule();
    pipelineShaderStageInfo.pName = "main";
}
