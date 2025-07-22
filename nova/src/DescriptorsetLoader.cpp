
#include "DescriptorsetLoader.hpp"
using json = nlohmann::json;

static VkShaderStageFlags ParseShaderStage(const std::string stage) {
    if (stage == "vertex") return	VK_SHADER_STAGE_VERTEX_BIT;
    if (stage == "fragment") return VK_SHADER_STAGE_FRAGMENT_BIT;
    if (stage == "geometry") return VK_SHADER_STAGE_GEOMETRY_BIT;
    if (stage == "compute") return VK_SHADER_STAGE_COMPUTE_BIT;
    if (stage == "tessellationControl") return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    if (stage == "tessellationEvaluation") return	VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    if (stage == "allGraphics") return VK_SHADER_STAGE_ALL_GRAPHICS;
    if (stage == "all") return VK_SHADER_STAGE_ALL;
    if (stage == "raygen") return	VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    if (stage == "anyHit") return	VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
    if (stage == "closestHit") return	VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    if (stage == "miss") return VK_SHADER_STAGE_MISS_BIT_KHR;
    if (stage == "intersection") return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
    if (stage == "callable") return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    if (stage == "task") return VK_SHADER_STAGE_TASK_BIT_EXT;
    if (stage == "mesh") return VK_SHADER_STAGE_MESH_BIT_EXT;

    throw std::runtime_error("Unsupported Shader stage in Graphics Config " + stage);
}

static VkDescriptorType ParseDescriptorType(const std::string descriptorType) {
    if (descriptorType == "uniformBuffer") return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    if (descriptorType == "uniformBufferDynamic") return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    if (descriptorType == "storageBuffer") return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    if (descriptorType == "storageBufferDynamic") return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    if (descriptorType == "sampler") return VK_DESCRIPTOR_TYPE_SAMPLER;
    if (descriptorType == "combinedImageSampler") return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;  
    if (descriptorType == "sampledImage") return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    if (descriptorType == "storageImage") return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    if (descriptorType == "uniformTexelBuffer") return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    if (descriptorType == "storageTexelBuffer") return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    if (descriptorType == "inputAttachment") return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    if (descriptorType == "inlineUniformBlock") return VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK;
    if (descriptorType == "accelerationStructure") return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    throw std::runtime_error("could not find Descriptor Type in Graphics Config: " + descriptorType);
}

static DesciptorUsageScope ParseUsageScope(const std::string scope) {
    if (scope == "perObject") { return DesciptorUsageScope::USAGE_SCOPE_PER_OBJECT; }
    else if (scope == "perFrame") { return DesciptorUsageScope::USAGE_SCOPE_PER_FRAME; }
    else if (scope == "singular") { return DesciptorUsageScope::USAGE_SCOPE_SINGULAR; }
    else if (scope == "global") { return DesciptorUsageScope::USAGE_SCOPE_GLOBAL; }
    else { throw std::runtime_error("could not find scope Type in Descriptor Config: " + scope); }
}

static uint32_t ParseInt(const std::string scope) {
    if (scope == "perObject") { return DesciptorUsageScope::USAGE_SCOPE_PER_OBJECT; }
    else if (scope == "perFrame") { return DesciptorUsageScope::USAGE_SCOPE_PER_FRAME; }
    else if (scope == "singular") { return DesciptorUsageScope::USAGE_SCOPE_SINGULAR; }
    else if (scope == "global") { return DesciptorUsageScope::USAGE_SCOPE_GLOBAL; }
    else { throw std::runtime_error("could not find scope Type in Descriptor Config: " + scope); }
}

DescriptorFile DescriptorsetLoader::LoadFromFile(std::string filePath){
    DescriptorFile file = {};
    std::vector<DescriptorSetInfo> descriptorInfoOut;
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open graphics config: " + filePath);
    }
    json j;
    in >> j;
    
    //for each descriptor set
    if (j.contains("descriptorSets")) {

        for (const auto& layout : j["descriptorSets"]) {

            DescriptorSetInfo info = {};
         
            info.scope = ParseUsageScope(layout["usageScope"]);
            std::vector<VkDescriptorSetLayoutBinding> descSetLayoutBindings = {};
            std::vector<std::optional<uint32_t>> requestedLayers;
            std::vector<std::optional<uint32_t>> requestedMiplevels;
            uint32_t binding = 0;
            std::vector<std::string> descriptorNames;
            //for each binding
            for (const auto& dsl : layout["descriptorSetLayout"]) {

                VkDescriptorSetLayoutBinding desclayoutbinding{};
                desclayoutbinding.binding = binding;
                desclayoutbinding.descriptorCount = dsl["descriptorCount"];
                desclayoutbinding.stageFlags = 0;
                for (const auto& stage : dsl["shaderStage"]) {
                    desclayoutbinding.stageFlags |= ParseShaderStage(stage);
                }
                auto descriptorType = ParseDescriptorType(dsl["descriptorType"]);

                if (descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE || descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                    uint32_t layers = dsl["layers"];
                    uint32_t mipLevels = dsl["mipLevels"];
                    requestedLayers.push_back(layers);
                    requestedMiplevels.push_back(mipLevels);
                }
                else {
                    requestedLayers.push_back(std::nullopt); 
                    requestedMiplevels.push_back(std::nullopt);
                }
                desclayoutbinding.descriptorType = descriptorType;
                info.descriptorCounts[descriptorType] += 1;

                descriptorNames.push_back(dsl["name"]);
                descSetLayoutBindings.push_back(desclayoutbinding);
                binding++;
            }
            info.descriptorBindings = descSetLayoutBindings;
            file.descriptorNames.push_back(descriptorNames);
            file.requestedLayers.push_back(requestedLayers);
            file.requestedMipLevels.push_back(requestedMiplevels);
            descriptorInfoOut.push_back(info);
            file.setsBindings.push_back(descSetLayoutBindings);
        }
    }
    file.descriptorSetsInfo = descriptorInfoOut;
    return file;
}