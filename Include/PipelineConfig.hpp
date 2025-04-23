#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cassert>

struct PipelineConfig {
    
    PipelineConfig():MAX_FRAMES(2){}
    PipelineConfig(uint32_t maxFrames):MAX_FRAMES(2){}

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    VkPipelineVertexInputStateCreateInfo vertexInput = {};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    VkViewport viewport = {};
    VkRect2D scissor = {};
    VkPipelineViewportStateCreateInfo viewportState = {};
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    VkPipelineColorBlendAttachmentState blendAttachment = {};
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    VkPipelineLayout pipelineLayout;
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = std::vector<VkDescriptorSetLayoutBinding>();
    std::vector<VkDescriptorSetLayoutCreateInfo> descriptorSetLayoutCreateInfos = std::vector<VkDescriptorSetLayoutCreateInfo>();
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts = std::vector<VkDescriptorSetLayout>();
    std::vector<VkDescriptorSet> descriptorSets = std::vector<VkDescriptorSet> ();  //per set per frame;
    std::vector<std::vector<VkDescriptorSet>> descriptorSetsPerFrame = std::vector<std::vector<VkDescriptorSet>> ();  //descriptorSetsPerFrame[frame] -> returns all descriptor sets for the frame
    std::vector<std::vector<VkDescriptorSet>> descriptorSetFrameDuplicates = std::vector<std::vector<VkDescriptorSet>> (); //descriptorSetFrameDuplicates -> Each descriptor set returns the duplicates of itself for other frames
    VkDescriptorPool descriptorPool;
    const uint32_t MAX_FRAMES;
    bool dynamicViewState = true;
    
    void SetupInputAssemblyState(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST) {
    
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = topology;
     
    }


    void SetupViewportState() {
        if(dynamicViewState){
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.scissorCount = 1;
            viewportState.pViewports = nullptr; 
            viewportState.pScissors = nullptr;  
        }
        else{
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;
        }
  
    }

    void SetupRasterizationState() {
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    }

    void SetupMultisampleState() {
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    }

    void SetupColorBlending() {
        blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &blendAttachment;
    }

    void CreatePipelineLayout(VkDevice& device){
        //no push constants as of now

        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
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

        dynamicViewState = false;
        SetupViewportState();
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
        samplerLayoutBinding.pImmutableSamplers = &sampler;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        descriptorSetLayoutBindings.push_back(samplerLayoutBinding);

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &samplerLayoutBinding;
        descriptorSetLayoutCreateInfos.push_back(layoutInfo);
    }
    void AllocateDescriptorSets(VkDevice& device){

        descriptorSets.clear();
        descriptorSets.resize(descriptorSetLayoutCreateInfos.size());
        
        descriptorSetsPerFrame.resize(MAX_FRAMES);
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
    
            descriptorSetFrameDuplicates[i].resize(MAX_FRAMES);
    
            if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSetFrameDuplicates[i].data()) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }

            for (size_t frameIndex = 0; frameIndex < MAX_FRAMES; ++frameIndex) {
                descriptorSetsPerFrame[frameIndex].push_back(descriptorSetFrameDuplicates[i][frameIndex]);
            }     

        }
        
    }
        

};