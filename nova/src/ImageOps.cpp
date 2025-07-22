#include "ImageOps.hpp"
#include "BufferOps.hpp"
#include "GPU.hpp"
#include "CommandManager.hpp"

VkBuffer ImageOps::stagingBuffer = VK_NULL_HANDLE;
VkDeviceMemory ImageOps::stagingMemory = VK_NULL_HANDLE;
VkDeviceSize ImageOps::currentStagingOffset = 0;
VkDeviceSize ImageOps::currentStagingAllocationSize = 1024 * 1024 * 128;

void ImageOps::CreateImage(
    std::shared_ptr<GPU> gpu,
    uint32_t width,
    uint32_t height,
    uint32_t mipLevels,
    uint32_t arrayLayers,
    VkSampleCountFlagBits samples,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImageType imageType,
    VkImageLayout initialLayout,
    VkImageCreateFlags createFlags,
    VkImage& image,
    VkDeviceMemory& imageMemory
) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = imageType;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = initialLayout;
    imageInfo.usage = usage;
    imageInfo.samples = samples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = createFlags;

    if (vkCreateImage(gpu->GetVkDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(gpu->GetVkDevice(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(gpu->GetPhysicalDevice(), &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            allocInfo.memoryTypeIndex = i;
            break;
        }
    }

    auto result = vkAllocateMemory(gpu->GetVkDevice(), &allocInfo, nullptr, &imageMemory);
    assert(result == VK_SUCCESS && "Create image failed");

    result = vkBindImageMemory(gpu->GetVkDevice(), image, imageMemory, 0);
    assert(result == VK_SUCCESS && "Bind Image Memory failed");
}

void ImageOps::CreateImageView(
    std::shared_ptr<GPU> gpu,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspectFlags,
    VkImageViewType viewType,
    uint32_t mipLevels,
    uint32_t arrayLayers,
    VkImageView& imageView
) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = viewType;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = arrayLayers;

    auto result = vkCreateImageView(gpu->GetVkDevice(), &viewInfo, nullptr, &imageView);
    assert(result == VK_SUCCESS && "Create image View failed");
}

void ImageOps::CreateSampler(
        std::shared_ptr<GPU> gpu, 
        uint32_t mipLevels, 
        VkFilter magFilter,
        VkFilter minFilter,
        VkSamplerMipmapMode mipmapmode,
        VkSamplerAddressMode addressModeU,
        VkSamplerAddressMode addressNodeV,
        VkSamplerAddressMode addressNodeW,
        float mipLodBias,
        VkBool32 anisotrophyEnable,
        float maxAnisotrophy,
        float minLod,
        float maxLod,
        VkBorderColor borderColor,
        VkBool32 unnormalizedCoordinates,
        VkSampler& sampler
    ) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = magFilter;
    samplerInfo.minFilter = minFilter;
    samplerInfo.mipmapMode = mipmapmode;
    samplerInfo.addressModeU = addressModeU;
    samplerInfo.addressModeV = addressNodeV;
    samplerInfo.addressModeW = addressNodeW;
    samplerInfo.mipLodBias = mipLodBias;
    samplerInfo.anisotropyEnable = anisotrophyEnable;
    samplerInfo.maxAnisotropy = maxAnisotrophy;
    samplerInfo.minLod = minLod;
    samplerInfo.maxLod = maxLod;
    samplerInfo.borderColor = borderColor;
    samplerInfo.unnormalizedCoordinates = unnormalizedCoordinates;

    auto result = vkCreateSampler(gpu->GetVkDevice(), &samplerInfo, nullptr, &sampler);
    assert(result == VK_SUCCESS && "Create image Sampler failed");
    
}

 void ImageOps::UploadToStaging(
    std::shared_ptr<GPU> gpu,
    VkDeviceSize imageSize,
    const uint8_t* pixels) {
    
    void* data;
    vkMapMemory(gpu->GetVkDevice(), ImageOps::stagingMemory, ImageOps::currentStagingOffset, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(gpu->GetVkDevice(), ImageOps::stagingMemory);
    ImageOps::currentStagingOffset += imageSize;
 }
 void ImageOps::CopyBufferToImage(
     std::shared_ptr<CommandManager> cmdManager,
     VkBuffer buffer,
     VkDeviceSize stagingBufferOffset,
     VkImage image,
     VkImageAspectFlags aspectMask, 
     uint32_t width,
     uint32_t height) {

     VkBufferImageCopy region{};
     region.bufferOffset = stagingBufferOffset; 
     region.bufferRowLength = 0; // tightly packed
     region.bufferImageHeight = 0; // tightly packed

     region.imageSubresource.aspectMask = aspectMask; // assuming color image
     region.imageSubresource.mipLevel = 0; // base mip level
     region.imageSubresource.baseArrayLayer = 0; // first layer
     region.imageSubresource.layerCount = 1; // one layer (no array)

     region.imageOffset = { 0, 0, 0 }; // start at top-left corner of the image
     region.imageExtent = { width, height, 1 }; // size of the region to copy

     auto commandBuffer = cmdManager->BeginSingleTimeCommands();
     vkCmdCopyBufferToImage(
         commandBuffer,
         buffer,
         image,
         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, // image must be in this layout for copy
         1,
         &region
     );
     cmdManager->EndSingleTimeCommands(commandBuffer);
 }

 void ImageOps::TransitionImageLayout(
        std::shared_ptr<GPU> gpu, 
        std::shared_ptr<CommandManager> cmdManager, 
        VkImage& image,
        VkFormat format,
        VkImageLayout currentLayout,
        VkImageLayout requestedLayout,
        uint32_t mipLevels,
        uint32_t layerCount) {

     auto HasStencilComponent = [](VkFormat format) {
         return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;};

     VkImageMemoryBarrier barrier{};
     barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
     barrier.oldLayout = currentLayout;
     barrier.newLayout = requestedLayout;

     barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
     barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

     barrier.image = image;
     if (requestedLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
         if (HasStencilComponent(format)) {
             barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
         }
         else {
             barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
         }
     }
     else {
         barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
     }

     barrier.subresourceRange.baseMipLevel = 0;
     barrier.subresourceRange.levelCount = mipLevels;
     barrier.subresourceRange.baseArrayLayer = 0;
     barrier.subresourceRange.layerCount = layerCount;

     // Guess sensible default pipeline stages and access masks
     VkPipelineStageFlags srcStage;
     VkPipelineStageFlags dstStage;

     if (currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && requestedLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

         barrier.srcAccessMask = 0;
         barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
         srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
         dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
     }
     else if (currentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && requestedLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {

         barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
         barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
         srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
         dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
     }
     else if (currentLayout == VK_IMAGE_LAYOUT_UNDEFINED && requestedLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {

         barrier.srcAccessMask = 0;
         barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
         srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
         dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
     }
     else {
         // Catch-all fallback (general read/write case)
         barrier.srcAccessMask = 0;
         barrier.dstAccessMask = 0;
         srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
         dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
     }

     auto cmdBuffer = cmdManager->BeginSingleTimeCommands();
     vkCmdPipelineBarrier(
         cmdBuffer,
         srcStage, dstStage,
         0,
         0, nullptr,
         0, nullptr,
         1, &barrier
     );
     cmdManager->EndSingleTimeCommands(cmdBuffer);
 }

 //What this code is responsible for — high level

 //    You have an image which can have multiple "aspects":

 //Color aspect — the usual color data in textures.

 //    Depth aspect — depth buffer data used for depth testing.

 //    Stencil aspect — stencil buffer data(sometimes combined with depth).

 //    The imageLayout(like VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) defines how you want to use the image as a whole — for example, as a depth / stencil buffer attachment, or as a sampled color texture.

 //    But the image’s memory is subdivided into these aspects, and Vulkan needs to know exactly which aspect(s) this transition applies to.

 //    So the code sets the aspectMask to tell Vulkan :

 //If you’re using the image as a depth - stencil attachment, specify the depth and, if present, stencil aspects.

 //    Otherwise, assume it’s a color image and specify the color aspect.

 //    Why doesn't imageLayout automatically specify aspects?

 //    Because VkImage can be complex :

 //Some image formats include both depth and stencil data — you might only want to transition one of those aspects at a time.

 //    The layout itself only indicates the general usage or purpose of the image, but the subresource range(including aspects) tells Vulkan exactly which part(s) of the image the barrier affects.

 //    This explicitness lets Vulkan be very precise and efficient.For example :

 //You can transition only the depth aspect without touching the stencil aspect if needed.

 //    Or handle individual mip levels or array layers separately.

 //    In summary

 //    The layout tells Vulkan how the image is going to be used globally.

 //    The aspect mask tells Vulkan which part of the image’s memory you’re talking about for this operation.

 //    This separation gives you fine - grained control over image memory transitions and synchronization.
 // srcAccessMask

 //    "What kind of accesses might still be happening in the old layout that I need to wait for?"

 //    This is about previous memory operations you want to wait for.

 //    Examples:

 //VK_ACCESS_TRANSFER_WRITE_BIT = wait for transfer writes to finish

 //    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT = wait for a render pass writing to a color target

 //    Think of this as "what was writing or reading before that I now need to wait on?"

 //    Only relevant if you're coming from a layout where data was being read or written
 //    (If old layout is UNDEFINED, you can safely set this to 0)
 //     dstAccessMask

 //    "What kind of accesses do I intend to do with the resource in the new layout, and want to make sure the memory is ready for?"

 //    This defines the type of memory access you want to do next.

 //    It also makes that memory visible to those accesses.

 //    Examples:

 //VK_ACCESS_SHADER_READ_BIT = prepare for shader sampling

 //    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT = prepare for writing depth

 //     Vulkan’s memory model requires explicit memory visibility, and this is how you declare it.
 //     srcStageMask

 //    "What stage(s) in the pipeline might still be using this image in its old layout?"

 //    This controls when the barrier happens with respect to earlier pipeline operations.

 //    It ensures that anything in these stages is finished before the barrier.

 //    Example:

 //VK_PIPELINE_STAGE_TRANSFER_BIT = wait for transfer to finish

 //    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = wait for render pass writes

 //     dstStageMask

 //    "What stage(s) in the pipeline do I want to use the resource in next?"

 //    This controls when the barrier releases the resource to be used in later stages.

 //    It delays execution of those stages until the memory is ready.

