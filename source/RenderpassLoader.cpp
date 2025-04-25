// RenderPassLoader.cpp
#include "RenderpassLoader.hpp"


using json = nlohmann::json;

static VkFormat ParseFormat(const std::string& str) {
    // Add more as needed
    //COLOR
    if (str == "VK_FORMAT_B8G8R8A8_UNORM") return VK_FORMAT_B8G8R8A8_UNORM;
    if (str == "VK_FORMAT_D32_SFLOAT") return VK_FORMAT_D32_SFLOAT;
    if (str =="VK_FORMAT_R8G8B8A8_UNORM") return VK_FORMAT_R8G8B8A8_UNORM;
    if (str =="VK_FORMAT_B8G8R8A8_UNORM") VK_FORMAT_B8G8R8A8_UNORM;
    if (str =="VK_FORMAT_R8G8B8A8_SRGB") return VK_FORMAT_R8G8B8A8_SRGB;
    if (str =="VK_FORMAT_B8G8R8A8_SRGB") return VK_FORMAT_B8G8R8A8_SRGB;
    if (str =="VK_FORMAT_R8_UNORM") return VK_FORMAT_R8_UNORM;
    if (str =="VK_FORMAT_R8G8_UNORM") return VK_FORMAT_R8G8_UNORM;
    if (str =="VK_FORMAT_R16G16B16A16_SFLOAT" ) return VK_FORMAT_R16G16B16A16_SFLOAT;
    if (str =="VK_FORMAT_R32G32B32A32_SFLOAT") return VK_FORMAT_R32G32B32A32_SFLOAT;
    if (str =="VK_FORMAT_B10G11R11_UFLOAT_PACK32") return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    if (str =="VK_FORMAT_R16_SFLOAT") return VK_FORMAT_R16_SFLOAT;
    if (str =="VK_FORMAT_D16_UNORM" ) return VK_FORMAT_D16_UNORM;
    if (str =="VK_FORMAT_D24_UNORM_S8_UINT") return VK_FORMAT_D24_UNORM_S8_UINT;
    if (str =="VK_FORMAT_D32_SFLOAT") return VK_FORMAT_D32_SFLOAT;
    if (str =="VK_FORMAT_D32_SFLOAT_S8_UINT") return VK_FORMAT_D32_SFLOAT_S8_UINT;
    if (str =="VK_FORMAT_S8_UINT") return VK_FORMAT_S8_UINT;
    if (str =="VK_FORMAT_R32_UINT") return VK_FORMAT_R32_UINT;
    if (str =="VK_FORMAT_R32G32_UINT") return VK_FORMAT_R32G32_UINT;
    if (str =="VK_FORMAT_R32G32B32_SFLOAT") return VK_FORMAT_R32G32B32_SFLOAT;
    if (str =="VK_FORMAT_R32G32B32A32_UINT") return VK_FORMAT_R32G32B32A32_UINT;
    if (str =="VK_FORMAT_BC3_UNORM_BLOCK") return VK_FORMAT_BC3_UNORM_BLOCK;

    //STENCILS
    if (str =="VK_FORMAT_S8_UINT") return VK_FORMAT_S8_UINT;
    if (str =="VK_FORMAT_D24_UNORM_S8_UINT") return VK_FORMAT_D24_UNORM_S8_UINT;
    if (str =="VK_FORMAT_D32_SFLOAT_S8_UINT") return VK_FORMAT_D32_SFLOAT_S8_UINT;

    //DEPTH
    if("VK_FORMAT_D16_UNORM") return VK_FORMAT_D16_UNORM;
    if("VK_FORMAT_D24_UNORM_S8_UINT") return VK_FORMAT_D24_UNORM_S8_UINT;
    if("VK_FORMAT_D32_SFLOAT") return VK_FORMAT_D32_SFLOAT;
    if("VK_FORMAT_D32_SFLOAT_S8_UINT") return VK_FORMAT_D32_SFLOAT_S8_UINT;
    throw std::runtime_error("Unsupported format: " + str);
}

static VkImageLayout ParseLayout(const std::string& str) {
    if (str == "VK_IMAGE_LAYOUT_UNDEFINED") return VK_IMAGE_LAYOUT_UNDEFINED;
    if (str == "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR") return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    if (str == "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL") return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL") return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    throw std::runtime_error("Unsupported layout: " + str);
}

static VkAttachmentLoadOp ParseLoadOp(const std::string& op) {
    if (op == "load") return VK_ATTACHMENT_LOAD_OP_LOAD;
    if (op == "clear") return VK_ATTACHMENT_LOAD_OP_CLEAR;
    return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

static VkAttachmentStoreOp ParseStoreOp(const std::string& op) {
    if (op == "store") return VK_ATTACHMENT_STORE_OP_STORE;
    return VK_ATTACHMENT_STORE_OP_DONT_CARE;
}
VkPipelineStageFlags ParseStageMask(const std::string op){
    VkPipelineStageFlags out = 0;
    if("VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT") out |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    if("VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT") out |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
    if("VK_PIPELINE_STAGE_VERTEX_INPUT_BIT") out |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    if("VK_PIPELINE_STAGE_VERTEX_SHADER_BIT") out |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    if("VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT") out |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
    if("VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT") out |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
    if("VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT") out |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
    if("VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT") out |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    if("VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT") out |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    if("VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT") out |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    if("VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT") out |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    if("VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT") out |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    if("VK_PIPELINE_STAGE_TRANSFER_BIT") out |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    if("VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT") out |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    if("VK_PIPELINE_STAGE_HOST_BIT") out |= VK_PIPELINE_STAGE_HOST_BIT;
    if("VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT") out |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
    if("VK_PIPELINE_STAGE_ALL_COMMANDS_BIT") out |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    return out;
}

VkAccessFlags ParseAcessMask(const std::string op){
    VkAccessFlags out = 0;
    if("VK_ACCESS_INDIRECT_COMMAND_READ_BIT") out |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    if("VK_ACCESS_INDEX_READ_BIT") out |= VK_ACCESS_INDEX_READ_BIT;
    if("VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT") out |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    if("VK_ACCESS_UNIFORM_READ_BIT") out |= VK_ACCESS_UNIFORM_READ_BIT;
    if("VK_ACCESS_INPUT_ATTACHMENT_READ_BIT") out |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    if("VK_ACCESS_SHADER_READ_BIT") out |= VK_ACCESS_SHADER_READ_BIT;
    if("VK_ACCESS_SHADER_WRITE_BIT") out |= VK_ACCESS_SHADER_WRITE_BIT;
    if("VK_ACCESS_COLOR_ATTACHMENT_READ_BIT") out |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    if("VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT") out |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    if("VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT") out |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    if("VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT") out |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    if("VK_ACCESS_TRANSFER_READ_BIT") out |= VK_ACCESS_TRANSFER_READ_BIT;
    if("VK_ACCESS_TRANSFER_WRITE_BIT") out |= VK_ACCESS_TRANSFER_WRITE_BIT;
    if("VK_ACCESS_HOST_READ_BIT") out |= VK_ACCESS_HOST_READ_BIT;
    if("VK_ACCESS_HOST_WRITE_BIT") out |= VK_ACCESS_HOST_WRITE_BIT;
    if("VK_ACCESS_MEMORY_READ_BIT") out |= VK_ACCESS_MEMORY_READ_BIT;
    if("VK_ACCESS_MEMORY_WRITE_BIT") out |= VK_ACCESS_MEMORY_WRITE_BIT;

    return out;
}

VkDependencyFlags ParseDepFlags(const std::string op){
    VkDependencyFlags out = 0;

    if("VK_DEPENDENCY_BY_REGION_BIT") out |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    if("VK_DEPENDENCY_VIEW_LOCAL_BIT") out |= VK_ACCESS_INDEX_READ_BIT;
    if("VK_DEPENDENCY_DEVICE_GROUP_BIT") out |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;


    return out;
}

VkSampleCountFlagBits ParseSamples(const int sampleCountStr) {

    if (sampleCountStr == 1) return VK_SAMPLE_COUNT_1_BIT;
    if (sampleCountStr == 2) return VK_SAMPLE_COUNT_2_BIT;
    if (sampleCountStr == 4) return VK_SAMPLE_COUNT_4_BIT;
    if (sampleCountStr == 8) return VK_SAMPLE_COUNT_8_BIT;
    if (sampleCountStr == 16) return VK_SAMPLE_COUNT_16_BIT;
    if (sampleCountStr == 32) return VK_SAMPLE_COUNT_32_BIT;
    if (sampleCountStr == 64) return VK_SAMPLE_COUNT_64_BIT;
  
    std::cerr << "Unknown sample count string: " << sampleCountStr << std::endl;
        return VK_SAMPLE_COUNT_1_BIT; // Default to 1 sample if invalid string

}

VkRenderPassCreateInfo RenderPassLoader::LoadFromFile(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open render pass file: " + filePath);
    }
    json j;
    in >> j;
    
    VkRenderPassCreateInfo out;
    out.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    std::vector<VkAttachmentDescription> descriptions;
    for (const auto& a : j["attachments"]) {
        VkAttachmentDescription desc{};
        desc.format = ParseFormat(a["format"]);
        desc.samples = ParseSamples(a["samples"]);
        desc.loadOp = ParseLoadOp(a["loadOp"]);
        desc.storeOp = ParseStoreOp(a["storeOp"]);
        desc.stencilLoadOp = ParseLoadOp(a.value("stencilLoadOp", "dont_care"));
        desc.stencilStoreOp = ParseStoreOp(a.value("stencilStoreOp", "dont_care"));
        desc.initialLayout = ParseLayout(a["initialLayout"]);
        desc.finalLayout = ParseLayout(a["finalLayout"]);
        descriptions.push_back(desc);
    }

    std::vector<VkSubpassDescription> subpasses;
    for (const auto& s : j["subpasses"]) {
        VkSubpassDescription sp{};
        sp.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        std::vector<VkAttachmentReference> colorAtachmentRefs;
        std::vector<VkAttachmentReference> depthAtachmentRefs;
        
        
        for (const auto& c : s["colorAttachments"]) {
           VkAttachmentReference colorAttachmentRef{};
           auto index = static_cast<uint32_t>(c["attachment"]);
           auto layout = ParseLayout(c["layout"]);
           colorAttachmentRef.attachment = index;
           colorAttachmentRef.layout = layout;
           colorAtachmentRefs.push_back(colorAttachmentRef);
           
        }
      
        if(s.contains("depthStencilAttachment")){
            for (const auto& c : s["depthStencilAttachment"]) {
                VkAttachmentReference depthAttachmentRef{};
                auto index = static_cast<uint32_t>(c["attachment"]);
                auto layout = ParseLayout(c["layout"]);
                depthAttachmentRef.attachment = index;
                depthAttachmentRef.layout = layout;
                depthAtachmentRefs.push_back(depthAttachmentRef);
                    
            }
        }
      
        sp.colorAttachmentCount = colorAtachmentRefs.size();
        sp.pColorAttachments = colorAtachmentRefs.data();
        if(depthAtachmentRefs.size() > 0){
            sp.pDepthStencilAttachment = &depthAtachmentRefs[0];
        }
        subpasses.push_back(sp);
        
    }
    std::vector<VkSubpassDependency> subpassDependencies;
    if (j.contains("dependencies")) {
        for (const auto& d : j["dependencies"]) {
            VkSubpassDependency dep{};
            dep.srcSubpass = d["srcSubpass"];
            dep.dstSubpass = d["dstSubpass"];
            dep.srcStageMask = ParseStageMask(d["srcStageMask"]);
            dep.dstStageMask = ParseStageMask(d["dstStageMask"]);
            dep.srcAccessMask = ParseAcessMask(d["srcAccessMask"]);
            dep.dstAccessMask = ParseAcessMask(d["dstAccessMask"]);
            dep.dependencyFlags = ParseDepFlags( d["dependencyFlags"]);
            subpassDependencies.push_back(dep);
        }
    }
    out.attachmentCount = descriptions.size();
    out.pAttachments = descriptions.data();
    out.subpassCount = subpasses.size();
    out.pSubpasses = subpasses.data();
    out.dependencyCount = subpassDependencies.size();
    out.pDependencies = subpassDependencies.data();

    return out;
}


VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice) {

    //tilling linear means the memory can be accessed by the cpu
    //tilling optimal means the memory is arranged for the gpu
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

static VkFormat findDepthFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates ) {
    return findSupportedFormat(
        candidates,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
        physicalDevice
    );
}

