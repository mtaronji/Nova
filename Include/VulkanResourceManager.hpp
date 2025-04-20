#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <cassert>
#include <glm/glm.hpp>

//The alignas ensures that the memory address for the start of the ubo is at a multiple of 16
//A breakdown would be something like this:
//start of struct is at a 16byte memory address. 
//then we jump forward 16 * 4 bytes for our view (safe because that is a multiple of 16)
//another 16 * 4 bytes we find our proj
//another 12 we find our camera position
//another 4 our padding (so we reach the end of the struct at a 16 byte boundary)


struct alignas(16)CameraUBO {
    glm::mat4 view;         //4 *4 * 4bytes = 64 = 16 * 4             offset = 0
    glm::mat4 proj;         //4 * 4 bytes = 64 = 16 * 4              offset = 64
    glm::vec3 cameraPosition;   //4 * 3 bytes = 12 bytes          offset = 128
    float padding1;          //4 bytes plus the 12 bytes above make it 16       offset = 140
};                                                                      //      offset = 144 for struct = multiple of 16

struct alignas(16) ModelUBO {
    glm::mat4 model;
};

struct alignas(16) LightUBO {
    glm::vec3 lightDirection;
    float intensity;         // could also be used for attenuation

    glm::vec3 lightColor;
    float padding2;          // pad to 16 bytes again
};

struct MaterialUBO {
    glm::vec4 baseColor;    // vec4 to match alignment
    float roughness;
    float metallic;
    glm::vec2 padding;      // pad to 16-byte boundary
};

struct TimeUBO {
    float time;
    float deltaTime;
    int frame;
    float padding; // pad to 16 bytes
};

struct PipelineInfo{

    VkPipelineVertexInputStateCreateInfo vertexInput;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly; 
    VkPipelineViewportStateCreateInfo viewportState; 
    VkPipelineRasterizationStateCreateInfo rasterizerInfo;   
    VkPipelineMultisampleStateCreateInfo multismaplingInfo;
    VkPipelineColorBlendAttachmentState blendAttachmentState;
    VkPipelineColorBlendAttachmentState colorBlendState;
    VkPipelineLayoutCreateInfo layoutInfo;
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = std::vector<VkDescriptorSetLayoutBinding>();
    std::vector<VkDescriptorSetLayoutCreateInfo> descriptorSetLayoutCreateInfos = std::vector<VkDescriptorSetLayoutCreateInfo>();
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts = std::vector<VkDescriptorSetLayout>();
    std::vector<std::vector<VkDescriptorSet>> descriptorSets = std::vector<std::vector<VkDescriptorSet>> ();  //per set per frame;
    VkDescriptorPool descriptorPool;
    const uint32_t MAX_FRAMES;
    
    PipelineInfo():MAX_FRAMES(2){}
    PipelineInfo(uint32_t maxFrames):MAX_FRAMES(maxFrames){

    }

    void SetViewState(std::vector<VkViewport> vps, std::vector<VkRect2D> scissors){

        assert(vps.size() == scissors.size());


        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = static_cast<uint32_t>(vps.size());;
        viewportState.pViewports = vps.data();
        viewportState.scissorCount = static_cast<uint32_t>(scissors.size());
        viewportState.pScissors = scissors.data();
        this->viewportState = viewportState;
    }

    void CreateDescriptorPool(VkDevice & device) {

        std::vector<VkDescriptorPoolSize> poolSizes = std::vector<VkDescriptorPoolSize>();
        for(const auto setLayoutBinding: descriptorSetLayoutBindings){
            VkDescriptorPoolSize poolInfo;
            poolInfo.descriptorCount = setLayoutBinding.descriptorCount * MAX_FRAMES;  //we expect to need this UBO for multiple frames
            poolInfo.type = setLayoutBinding.descriptorType;
            poolSizes.push_back(poolInfo);
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES);

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }
    template<typename T>
    void SetDescriptorSetLayoutUBOInfo(const std::vector<T> ubodata, VkSampler & sampler = VK_NULL_HANDLE){

        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = static_cast<uint32_t>(descriptorSetLayoutCreateInfos.size() + 1);
        uboLayoutBinding.descriptorCount = static_cast<uint32_t>(ubodata.size());
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = &sampler;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        descriptorSetLayoutBindings.push_back(uboLayoutBinding);
   
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;
        descriptorSetLayoutCreateInfos.push_back(layoutInfo);
    }

   
    template<typename T>
    void SetDescriptorSetLayoutImageSamplerInfo(const std::vector<T> ubodata, VkSampler & sampler = VK_NULL_HANDLE){

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = static_cast<uint32_t>(descriptorSetLayoutCreateInfos.size() + 1);
        samplerLayoutBinding.descriptorCount = static_cast<uint32_t>(ubodata.size());
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = sampler;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        descriptorSetLayoutBindings.push_back(samplerLayoutBinding);

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &samplerLayoutBinding;
        descriptorSetLayoutCreateInfos.push_back(layoutInfo);
    }
    void CreateDescriptorSets(VkDevice& device){

        descriptorSets.clear();
        descriptorSets.resize(descriptorSetLayoutCreateInfos.size());
    
        for (size_t i = 0; i < descriptorSetLayoutCreateInfos.size(); ++i) {
            // Create actual descriptor set layout
            VkDescriptorSetLayout layout;
            if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfos[i], nullptr, &layout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor set layout!");
            }
    
            // Store layout for pipeline layout creation
            descriptorSetLayouts.push_back(layout);
    
            // make MAX_FRAMES copies of the layout object we just created
            std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES, layout);
    
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = descriptorPool;
            allocInfo.descriptorSetCount = MAX_FRAMES;
            allocInfo.pSetLayouts = layouts.data();
    
            descriptorSets[i].resize(MAX_FRAMES);
    
            if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets[i].data()) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }
        }     
    }
        

};

