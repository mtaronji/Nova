#include "Shader.hpp"
#include "GPU.hpp"
#include <fstream>

template<VkShaderStageFlagBits T>
Shader<T>::Shader(const GPU& gpu, const std::string& filepath) : gpu(gpu) {

    auto code = readFile(filepath);  
    CreateShaderModule(code);
    CreateShaderStageInfo();
}

template<VkShaderStageFlagBits T>
Shader<T>::~Shader() {
    if (shaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(gpu.GetVkDevice(), shaderModule, nullptr);
    }
}

template<VkShaderStageFlagBits T>
std::vector<char> Shader<T>::ReadFile(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open shader file: " + path);

    size_t size = (size_t)file.tellg();
    std::vector<char> buffer(size);
    file.seekg(0);
    file.read(buffer.data(), size);
    return buffer;
}
template<VkShaderStageFlagBits T>
void Shader<T>::CreateShaderModule(const std::vector<char>& code) {

    VkShaderModuleCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = code.size();
    info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(gpu.GetVkDevice(), &info, nullptr, &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module.");
}
template<VkShaderStageFlagBits T>
VkPipelineShaderStageCreateInfo Shader<T>:: GetPipelineStageInfo() const {
    return this->stage;
}

template<VkShaderStageFlagBits T>
void Shader<T>::CreateShaderStageInfo(){
    
    stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage.stage = T;
    stage.module = GetModule();
    stage.pName = "main";
}
