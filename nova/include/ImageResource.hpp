#pragma once

#include "Resource.hpp"
#include <vulkan/vulkan.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstddef>
#include <cstdint>
class GPU;
class CommandManager;

struct MipLayer {
    uint32_t mipLevels;
    uint32_t layers;

    // Equality operator
    bool operator==(const MipLayer& other) const {
        return mipLevels == other.mipLevels && layers == other.layers;
    }
};
struct ImageBundle {
    VkImage Image;
    VkImageView ImageView;
    VkSampler Sampler;
    VkDeviceMemory memory;
};
namespace std {
    template<>
    struct hash<MipLayer> {
        //0x85ebca6b and 0xc2b2ae35 are MurmurHash3 - derived constants.
        std::size_t operator()(const MipLayer& k) const noexcept {
            uint64_t hash = static_cast<uint64_t>(k.mipLevels) * 0x85ebca6b +
                            static_cast<uint64_t>(k.layers) * 0xc2b2ae35;
            hash ^= (hash >> 33);
            hash *= 0xff51afd7ed558ccd;
            hash ^= (hash >> 33);
            return static_cast<std::size_t>(hash);
        }
    };
}
//this is a class for textures
class ImageResource : public Resource {
public:
    ImageResource() = delete;
    ImageResource(std::shared_ptr<GPU> gpu, std::shared_ptr<CommandManager> cmdManager, const char* imageLocation, uint32_t copies, VkMemoryPropertyFlags memoryProperties);
    ~ImageResource() override {
        
    }
    void CreateTexture(uint32_t mipLevel, uint32_t layers);
    void Cleanup() override;

    ImageBundle GetTextureResolution(MipLayer mipLayer) {
        auto img = textureLayers.find(mipLayer);
        if (img == textureLayers.end()) {
            CreateTexture(mipLayer.mipLevels, mipLayer.layers);
            Copies++;
            return textureLayers[mipLayer];
        }
        else {
            return img->second;
        }
    }
    VkImageUsageFlags Usage;
    VkImageLayout ImageLayout;
    std::vector<VkImageView> ImageViews;
    std::vector<VkImage> Images;
    std::vector<VkSampler> Samplers;
    std::vector<uint32_t>  Layers;
    std::vector<uint32_t>  MipLevels;
    std::vector<VkDeviceMemory> MemoryAllocations;
    uint8_t* ImageData;

protected:
    const char* imageLocation;
    int width;
    int height;
    std::unordered_map<MipLayer, ImageBundle> textureLayers;
    std::shared_ptr<CommandManager> cmdManager;

};