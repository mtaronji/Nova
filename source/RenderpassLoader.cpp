// RenderPassLoader.cpp
#include "RenderpassLoader.hpp"


using json = nlohmann::json;

static VkFormat ParseFormat(const std::string& str) {
    // Add more as needed
    if (str == "VK_FORMAT_B8G8R8A8_UNORM") return VK_FORMAT_B8G8R8A8_UNORM;
    if (str == "VK_FORMAT_D32_SFLOAT") return VK_FORMAT_D32_SFLOAT;
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

RenderPassJson RenderPassLoader::LoadFromFile(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open render pass file: " + filePath);
    }
    json j;
    in >> j;
    
    RenderPassJson out;

    for (const auto& a : j["attachments"]) {
        AttachmentDescriptionJson desc{};
        desc.format = ParseFormat(a["format"]);
        desc.samples = static_cast<VkSampleCountFlagBits>(a.value("samples", 1));
        desc.loadOp = ParseLoadOp(a["loadOp"]);
        desc.storeOp = ParseStoreOp(a["storeOp"]);
        desc.stencilLoadOp = ParseLoadOp(a.value("stencilLoadOp", "dont_care"));
        desc.stencilStoreOp = ParseStoreOp(a.value("stencilStoreOp", "dont_care"));
        desc.initialLayout = ParseLayout(a["initialLayout"]);
        desc.finalLayout = ParseLayout(a["finalLayout"]);
        out.attachments.push_back(desc);
    }

    for (const auto& s : j["subpasses"]) {
        SubpassDescriptionJson sp{};
        sp.bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        for (const auto& c : s["colorAttachments"]) {
            sp.colorAttachments.push_back({
                c["attachment"], ParseLayout(c["layout"])
            });
        }
        if (s.contains("depthStencilAttachment")) {
            auto& d = s["depthStencilAttachment"];
            sp.depthStencilAttachment = AttachmentReferenceJson{
                d["attachment"], ParseLayout(d["layout"])
            };
        }
        out.subpasses.push_back(sp);
    }

    if (j.contains("dependencies")) {
        for (const auto& d : j["dependencies"]) {
            SubpassDependencyJson dep{};
            dep.srcSubpass = d["srcSubpass"];
            dep.dstSubpass = d["dstSubpass"];
            dep.srcStageMask = 0;  // Could parse enum list
            dep.dstStageMask = 0;
            dep.srcAccessMask = 0;
            dep.dstAccessMask = 0;
            dep.dependencyFlags = 0;
            out.dependencies.push_back(dep);
        }
    }

    return out;
}
