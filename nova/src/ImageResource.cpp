#include "ImageResource.hpp"
#include "ImageOps.hpp"
#include "GPU.hpp"
#include "CommandManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void ImageResource::Cleanup() {

    for (auto& [key, val] : textureLayers) {
        vkDestroyImageView(gpu->GetVkDevice(), val.ImageView, nullptr);
        vkDestroySampler(gpu->GetVkDevice(), val.Sampler, nullptr);
        vkDestroyImage(gpu->GetVkDevice(), val.Image, nullptr);
        vkFreeMemory(gpu->GetVkDevice(), val.memory, nullptr);
    }

    if (ImageData) {
        ImageData = nullptr;
    }

    if (Data) {
        free(Data);
        Data = nullptr;
    }
}

// image resources will always default to 
ImageResource::ImageResource(std::shared_ptr<GPU> gpu, std::shared_ptr<CommandManager> cmdManager, const char* imageLocation, uint32_t copies, VkMemoryPropertyFlags memoryProperties)
    :Resource(gpu,copies,memoryProperties), Usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT), ImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL), imageLocation(imageLocation)
        ,cmdManager(cmdManager) {

    this->resourceType = ResourceType::IMAGE;

    int width, height, channels; uint8_t* image_data = nullptr;
    image_data = stbi_load(imageLocation, &width, &height, &channels, STBI_rgb_alpha);
    assert(image_data && "image data did not successfully load");

    this->width = width; this->height = height; this->ImageData = image_data;
    
    for (uint32_t i = 0; i < copies - 1; i++) {
        CreateTexture(1, 1);
    }
}

void ImageResource::CreateTexture(uint32_t mipLevels, uint32_t layers) {

    assert(ImageData && "Image Data Missing. Object was already destroyed? ");
    VkSampler sampler = VK_NULL_HANDLE;
    VkImage image = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    //create resources for each copy requested and each layer
    
    ImageOps::UploadToStaging(gpu, width * height * 4, ImageData);


    ImageOps::CreateImage(gpu, width, height,
        mipLevels,                               //mip levels
        layers,                               //array layers                                                                      
        VK_SAMPLE_COUNT_1_BIT,           // samples (no MSAA)
        VK_FORMAT_R8G8B8A8_SRGB,         // format (common 4-channel sRGB)
        VK_IMAGE_TILING_OPTIMAL,         // tiling (optimal for GPU sampling)
        Usage, // usage (will copy data & sample)
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,  // properties (GPU local memory)
        VK_IMAGE_TYPE_2D,                // imageType (2D texture)
        VK_IMAGE_LAYOUT_UNDEFINED,       // initialLayout (undefined before transition)
        0,                              // createFlags (no special flags)
        image,
        memory);

    //transition the image and then copy the buffer then transition to the final shader read only format
    ImageOps::TransitionImageLayout(gpu, cmdManager, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, layers);
    ImageOps::CopyBufferToImage(cmdManager, ImageOps::stagingBuffer, 0, image, VK_IMAGE_ASPECT_COLOR_BIT, width, height);
    ImageOps::TransitionImageLayout(gpu, cmdManager, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels, layers);

    ImageOps::CreateImageView(gpu, image,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_VIEW_TYPE_2D_ARRAY,
        mipLevels,        // mipLevels
        layers,        // arrayLayers
        imageView
    );

    ImageOps::CreateSampler(gpu,
        mipLevels,
        VK_FILTER_LINEAR,
        VK_FILTER_LINEAR,
        VK_SAMPLER_MIPMAP_MODE_LINEAR,
        VK_SAMPLER_ADDRESS_MODE_REPEAT,
        VK_SAMPLER_ADDRESS_MODE_REPEAT,
        VK_SAMPLER_ADDRESS_MODE_REPEAT,
        0.0f,
        VK_TRUE,
        16.0f,
        0.0f,
        1.0f,
        VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        VK_FALSE,
        sampler
    );

    assert(sampler != VK_NULL_HANDLE && image != VK_NULL_HANDLE && imageView != VK_NULL_HANDLE);
    MipLayer key{ .mipLevels = mipLevels, .layers = layers };
    ImageBundle v{ .Image = image, .ImageView = imageView, .Sampler = sampler, .memory = memory };
    textureLayers.insert({ key, v });
}


