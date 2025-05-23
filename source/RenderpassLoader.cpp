// RenderPassLoader.cpp
#include "RenderpassLoader.hpp"


using json = nlohmann::json;

static VkFormat ParseFormat(const std::string& str) {
    // Add more as needed
    //COLOR
    if (str =="VK_FORMAT_R8G8B8A8_UNORM") return VK_FORMAT_R8G8B8A8_UNORM;
    if (str =="VK_FORMAT_B8G8R8A8_UNORM") return VK_FORMAT_B8G8R8A8_UNORM;
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
    if(str =="VK_FORMAT_D16_UNORM") return VK_FORMAT_D16_UNORM;
    if(str =="VK_FORMAT_D24_UNORM_S8_UINT") return VK_FORMAT_D24_UNORM_S8_UINT;
    if(str =="VK_FORMAT_D32_SFLOAT") return VK_FORMAT_D32_SFLOAT;
    if(str =="VK_FORMAT_D32_SFLOAT_S8_UINT") return VK_FORMAT_D32_SFLOAT_S8_UINT;
    throw std::runtime_error("Unsupported format: " + str);
}

static VkImageLayout ParseLayoutForAttachmentDescription(const std::string& str) {
    if (str == "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL") return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_GENERAL") return VK_IMAGE_LAYOUT_GENERAL;
    if (str == "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL") return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL") return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL") return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL") return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL") return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL") return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_UNDEFINED") return VK_IMAGE_LAYOUT_UNDEFINED;
    if (str == "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR") return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR ;
    throw std::runtime_error("Unsupported format: " + str);
}
static VkImageLayout ParseLayoutForAttachmentReference(const std::string& str) {
    if (str == "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL") return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_GENERAL") return VK_IMAGE_LAYOUT_GENERAL;
    if (str == "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL") return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL") return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL") return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL") return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL") return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    if (str == "VK_IMAGE_LAYOUT_UNDEFINED") return VK_IMAGE_LAYOUT_UNDEFINED;
    throw std::runtime_error("Unsupported format: " + str);

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

static VkImageUsageFlags ParseImageUse(const std::string& usage){
    if(usage == "VK_IMAGE_USAGE_TRANSFER_SRC_BIT") {return VK_IMAGE_USAGE_TRANSFER_SRC_BIT;}
    if(usage =="VK_IMAGE_USAGE_TRANSFER_DST_BIT"){return VK_IMAGE_USAGE_TRANSFER_DST_BIT;}
    if(usage =="VK_IMAGE_USAGE_SAMPLED_BIT"){return VK_IMAGE_USAGE_SAMPLED_BIT;}
    if(usage =="VK_IMAGE_USAGE_STORAGE_BIT"){return VK_IMAGE_USAGE_STORAGE_BIT;}
    if(usage =="VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT"){return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;}
    if(usage =="VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT"){return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;}
    if(usage =="VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT"){return VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;}
    if(usage =="VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT"){return VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;}
    if(usage =="VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR"){return VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR;}
    if(usage =="VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR"){return VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR;}
    if(usage =="VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR"){return VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR;}
    if(usage =="VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT"){return VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT;}
    if(usage =="VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR"){return VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;}
    if(usage =="VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT"){return VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT;}
    if(usage =="VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT"){return VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT;}
    if(usage =="VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI"){return VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI;}
    if(usage =="VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM"){return VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM;}
    if(usage =="VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM"){return VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM;}
    throw std::runtime_error("Unsupported imageUser: " + usage);
}
VkPipelineStageFlags ParseStageMask(const std::string op){

    if(op == "VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT") return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    if(op == "VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT") return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
    if(op == "VK_PIPELINE_STAGE_VERTEX_INPUT_BIT") return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    if(op == "VK_PIPELINE_STAGE_VERTEX_SHADER_BIT") return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    if(op == "VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT") return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
    if(op == "VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT") return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
    if(op == "VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT") return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
    if(op == "VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT") return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    if(op == "VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT") return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    if(op == "VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT") return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    if(op == "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT") return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    if(op == "VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT") return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    if(op == "VK_PIPELINE_STAGE_TRANSFER_BIT") return VK_PIPELINE_STAGE_TRANSFER_BIT;
    if(op == "VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT") return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    if(op == "VK_PIPELINE_STAGE_HOST_BIT") return VK_PIPELINE_STAGE_HOST_BIT;
    if(op == "VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT") return VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
    if(op == "VK_PIPELINE_STAGE_ALL_COMMANDS_BIT") return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    throw std::runtime_error("Unsupported format: " + op);
}

VkAccessFlags ParseAcessMask(const std::string op){
    if(op == "0") return 0;
    if(op == "VK_ACCESS_INDIRECT_COMMAND_READ_BIT") return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    if(op == "VK_ACCESS_INDEX_READ_BIT") return VK_ACCESS_INDEX_READ_BIT;
    if(op == "VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT") return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    if(op == "VK_ACCESS_UNIFORM_READ_BIT") return VK_ACCESS_UNIFORM_READ_BIT;
    if(op == "VK_ACCESS_INPUT_ATTACHMENT_READ_BIT") return VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    if(op == "VK_ACCESS_SHADER_READ_BIT") return VK_ACCESS_SHADER_READ_BIT;
    if(op == "VK_ACCESS_SHADER_WRITE_BIT") return VK_ACCESS_SHADER_WRITE_BIT;
    if(op == "VK_ACCESS_COLOR_ATTACHMENT_READ_BIT") return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    if(op == "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT") return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    if(op == "VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT") return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    if(op == "VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT") return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    if(op == "VK_ACCESS_TRANSFER_READ_BIT") return VK_ACCESS_TRANSFER_READ_BIT;
    if(op == "VK_ACCESS_TRANSFER_WRITE_BIT") return VK_ACCESS_TRANSFER_WRITE_BIT;
    if(op == "VK_ACCESS_HOST_READ_BIT") return VK_ACCESS_HOST_READ_BIT;
    if(op == "VK_ACCESS_HOST_WRITE_BIT") return VK_ACCESS_HOST_WRITE_BIT;
    if(op == "VK_ACCESS_MEMORY_READ_BIT") return VK_ACCESS_MEMORY_READ_BIT;
    if(op == "VK_ACCESS_MEMORY_WRITE_BIT") return VK_ACCESS_MEMORY_WRITE_BIT;
   

    throw std::runtime_error("Unsupported format: " + op);
}

VkDependencyFlags ParseDepFlags(const std::string op){
 
    if(op == "VK_DEPENDENCY_BY_REGION_BIT") return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    if(op == "VK_DEPENDENCY_VIEW_LOCAL_BIT") return VK_ACCESS_INDEX_READ_BIT;
    if(op == "VK_DEPENDENCY_DEVICE_GROUP_BIT") return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;


    throw std::runtime_error("Unsupported format: " + op);
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

uint32_t ParseSubpassIndexStr(const std::string index) {
    
    if (index == "VK_SUBPASS_EXTERNAL") {
        return VK_SUBPASS_EXTERNAL;
    }

    // fallback: treat string as a number
    try {
        return static_cast<uint32_t>(std::stoi(index));
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid subpass index string: " + index);
    }

}

VkAttachmentDescriptionFlags ParseFlags(const std::string flag) {
    VkAttachmentDescriptionFlags flags = 0;
    
    if (flag == "MAY_ALIAS") {
        flags |= VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
    }
    
    return flags;
}
VkImageAspectFlags ParseImageAspectFlags(const std::string flag){
    if(flag == "VK_IMAGE_ASPECT_COLOR_BIT"){return VK_IMAGE_ASPECT_COLOR_BIT;}
    if(flag == "VK_IMAGE_ASPECT_DEPTH_BIT"){return VK_IMAGE_ASPECT_DEPTH_BIT;}
    if(flag == "VK_IMAGE_ASPECT_STENCIL_BIT"){return VK_IMAGE_ASPECT_STENCIL_BIT;}	
    if(flag == "VK_IMAGE_ASPECT_METADATA_BIT"){return VK_IMAGE_ASPECT_METADATA_BIT;}
    if(flag == "VK_IMAGE_ASPECT_PLANE_0_BIT "){return VK_IMAGE_ASPECT_PLANE_0_BIT ; }
    throw std::runtime_error("Invalid parse image aspect: " + flag);
}


VkRenderPassCreateInfo RenderPassLoader::LoadFromFile(const std::string& filePath, 
                                                        std::vector<VkAttachmentDescription>& attachmentDescriptionsOut,
                                                        std::vector<VkSubpassDescription>& subpassDescriptionsOut,
                                                        std::vector<VkSubpassDependency>& subpassdependenciesOut,
                                                        std::vector<VkAttachmentReference>& colorAtachmentRefsOut,
                                                        std::vector<VkAttachmentReference>& depthAtachmentRefsOut,
                                                        std::vector<VkAttachmentReference>& resolveAtachmentRefsOut,
                                                        std::vector<VkImageUsageFlags>& imageUsesForAttachmentsOut,
                                                        std::vector<VkImageAspectFlags>& aspectFlagsForAttachmentsOut) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open render pass file: " + filePath);
    }
    json j;
    in >> j;
    
    VkRenderPassCreateInfo out = {};
    out.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    for (const auto& a : j["attachments"]) {
        VkAttachmentDescription desc{};
        
        desc.format = ParseFormat(a["format"]);
        desc.samples = ParseSamples(a["samples"]);
        desc.loadOp = ParseLoadOp(a["loadOp"]);
        desc.storeOp = ParseStoreOp(a["storeOp"]);
        desc.stencilLoadOp = ParseLoadOp(a.value("stencilLoadOp", "dont_care"));
        desc.stencilStoreOp = ParseStoreOp(a.value("stencilStoreOp", "dont_care"));
        desc.initialLayout = ParseLayoutForAttachmentDescription(a["initialLayout"]);
        desc.finalLayout = ParseLayoutForAttachmentDescription(a["finalLayout"]);

        if(a.contains("flags")){
            desc.flags = ParseFlags(a["flags"]);
        }
        else{
            desc.flags = 0;
        }
        VkImageUsageFlags uses = 0;
        for (const auto& u : a["imageUsage"]){
            uses |= ParseImageUse(u);
        }
        imageUsesForAttachmentsOut.push_back(uses);
        VkImageAspectFlags aspectflag = 0;
        for (const auto& f : a["imageAspectFlags"]){
            aspectflag |= ParseImageAspectFlags(f);
        }
        aspectFlagsForAttachmentsOut.push_back(aspectflag);
        attachmentDescriptionsOut.push_back(desc);

        // if(a["attachmentType"] == "color"){colorAttachmentDescriptionsOut.push_back(desc);} 
        // if(a["attachmentType"] == "depth"){depthAttachmentDescriptionsOut.push_back(desc);}
        // if(a["attachmentType"] == "stencil"){stencilAttachmentDescriptionsOut.push_back(desc);}
        // if(a["attachmentType"] == "input"){inputAttachmentDescriptionsOut.push_back(desc);}
        // if(a["attachmentType"] == "resolve"){resolveAttachmentDescriptionsOut.push_back(desc);}
    }


    for (const auto& s : j["subpasses"]) {
        VkSubpassDescription sp{};
       
        sp.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        
        
        for (const auto& c : s["colorAttachments"]) {
           VkAttachmentReference colorAttachmentRef{};
           auto index = static_cast<uint32_t>(c["attachment"]);
           auto layout = ParseLayoutForAttachmentReference(c["layout"]);
           colorAttachmentRef.attachment = index;
           colorAttachmentRef.layout = layout;
           colorAtachmentRefsOut.push_back(colorAttachmentRef);
           
        }
      
        if(s.contains("depthStencilAttachments")){
            for (const auto& c : s["depthStencilAttachments"]) {
                VkAttachmentReference depthAttachmentRef{};
                auto index = static_cast<uint32_t>(c["attachment"]);
                auto layout = ParseLayoutForAttachmentReference(c["layout"]);
                depthAttachmentRef.attachment = index;
                depthAttachmentRef.layout = layout;
                depthAtachmentRefsOut.push_back(depthAttachmentRef);
                    
            }
        }
        if(s.contains("resolveAttachments")){
            for (const auto& c : s["resolveAttachments"]) {
                VkAttachmentReference resolveAttachmentRef{};
                auto index = static_cast<uint32_t>(c["attachment"]);
                auto layout = ParseLayoutForAttachmentReference(c["layout"]);
                resolveAttachmentRef.attachment = index;
                resolveAttachmentRef.layout = layout;
                resolveAtachmentRefsOut.push_back(resolveAttachmentRef);
                    
            }
        }
      
        sp.colorAttachmentCount = colorAtachmentRefsOut.size();
        sp.pColorAttachments = colorAtachmentRefsOut.data();
        if(depthAtachmentRefsOut.size() > 0){
            sp.pDepthStencilAttachment = &depthAtachmentRefsOut[0];
        }
        if(resolveAtachmentRefsOut.size() > 0){
            sp.pResolveAttachments = resolveAtachmentRefsOut.data();
        }
        subpassDescriptionsOut.push_back(sp);
        
    }
 
    if (j.contains("dependencies")) {
        for (const auto& d : j["dependencies"]) {
            VkSubpassDependency dep{};
            dep.srcSubpass = ParseSubpassIndexStr(d["srcSubpass"]);       
            dep.dstSubpass = ParseSubpassIndexStr(d["dstSubpass"]);     
                   
            for (const auto& m : d["srcStageMask"]) {
                dep.srcStageMask |= ParseStageMask(m);
            }
            for (const auto& m : d["srcAccessMask"]) {
                dep.srcAccessMask |= ParseAcessMask(m);
            }
            for (const auto& m : d["dstStageMask"]) {
                dep.dstStageMask |= ParseStageMask(m);
            }
         
            for (const auto& m : d["dstAccessMask"]) {
                dep.dstAccessMask |= ParseAcessMask(m);
            }
            
            for (const auto& m : d["dependencyFlags"]) {
                dep.dependencyFlags |= ParseDepFlags(m);
            }
            
            subpassdependenciesOut.push_back(dep);
        }
    }
    out.attachmentCount = attachmentDescriptionsOut.size();
    out.pAttachments = attachmentDescriptionsOut.data();
    out.subpassCount = subpassDescriptionsOut.size();
    out.pSubpasses = subpassDescriptionsOut.data();
    out.dependencyCount = subpassdependenciesOut.size();
    out.pDependencies = subpassdependenciesOut.data();

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