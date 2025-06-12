#include "DescriptorsetLoader.hpp"
using json = nlohmann::json;

static VkShaderStageFlags ParseShaderStage(const std::string stage) {
    if ("vertex") return	VK_SHADER_STAGE_VERTEX_BIT;
    if ("fragment") return VK_SHADER_STAGE_FRAGMENT_BIT;
    if ("geometry") return VK_SHADER_STAGE_GEOMETRY_BIT;
    if ("compute") return VK_SHADER_STAGE_COMPUTE_BIT;
    if ("tessellationControl") return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    if ("tessellationEvaluation") return	VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    if ("allGraphics") return VK_SHADER_STAGE_ALL_GRAPHICS;
    if ("all") return VK_SHADER_STAGE_ALL;
    if ("raygen") return	VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    if ("anyHit") return	VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
    if ("closestHit") return	VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    if ("miss") return VK_SHADER_STAGE_MISS_BIT_KHR;
    if ("intersection") return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
    if ("callable") return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    if ("task") return VK_SHADER_STAGE_TASK_BIT_EXT;
    if ("mesh") return VK_SHADER_STAGE_MESH_BIT_EXT;

    throw std::runtime_error("Unsupported Shader stage in Graphics Config " + stage);
}

static VkDescriptorType ParseDescriptorType(const std::string descriptorType) {
    if ("uniformBuffer") return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    if ("uniformBufferDynamic") return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    if ("storageBuffer") return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    if ("storageBufferDynamic") return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    if ("sampler") return VK_DESCRIPTOR_TYPE_SAMPLER;
    if ("combinedImageSampler") return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    if ("sampledImage") return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    if ("storageImage") return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    if ("uniformTexelBuffer") return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    if ("storageTexelBuffer") return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    if ("inputAttachment") return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    if ("inlineUniformBlock") return VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK;
    if ("accelerationStructure") return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    throw std::runtime_error("could not find Descriptor Type in Graphics Config: " + descriptorType);
}

static DesciptorUsageScope ParseUsageScope(const std::string scope) {
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
 
    if (j.contains("descriptorSets")) {

        for (const auto& layout : j["descriptorSets"]) {

            DescriptorSetInfo info = {};
         
            info.scope = ParseUsageScope(layout["usageScope"]);
            std::vector<VkDescriptorSetLayoutBinding> descSetLayoutBindings = {};
            uint32_t binding = 0;
            std::vector<std::string> descriptorNames;
            for (const auto& dsl : layout["descriptorSetLayout"]) {

                VkDescriptorSetLayoutBinding desclayoutbinding{};
                desclayoutbinding.binding = binding;
                desclayoutbinding.descriptorCount = dsl["descriptorCount"];
                desclayoutbinding.stageFlags = 0;
                for (const auto& stage : dsl["shaderStage"]) {
                    desclayoutbinding.stageFlags |= ParseShaderStage(stage);
                }
                auto descriptorType = ParseDescriptorType(dsl["descriptorType"]);
                desclayoutbinding.descriptorType = descriptorType;
                info.descriptorCounts[descriptorType] += 1;

                descriptorNames.push_back(dsl["name"]);
                descSetLayoutBindings.push_back(desclayoutbinding);
                binding++;
            }
            info.descriptorBindings = descSetLayoutBindings;
            file.descriptorNames.push_back(descriptorNames);
            descriptorInfoOut.push_back(info);
            file.setsBindings.push_back(descSetLayoutBindings);
        }
    }
    file.descriptorSetsInfo = descriptorInfoOut;
    return file;
}