#include <vulkan/vulkan.h>

class PipelineConfig {
    public:
        PipelineConfig() {

            // Setup default states here
            vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = 800.0f;
            viewport.height = 600.0f;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
    
            scissor.extent.width = 800;
            scissor.extent.height = 600;
            scissor.offset = {0, 0};
        }
        VkPipelineInputAssemblyStateCreateInfo SetupInputAssemblyState(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST) {
            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = topology;
            return inputAssembly;
        }
        void SetVertexInput(); 
        VkPipelineVertexInputStateCreateInfo vertexInput = {};
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        VkViewport viewport = {};
        VkRect2D scissor = {};
        VkPipelineViewportStateCreateInfo viewportState = {};
        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        VkPipelineMultisampleStateCreateInfo multisampling = {};
        VkPipelineColorBlendAttachmentState blendAttachment = {};
        VkPipelineColorBlendStateCreateInfo colorBlending = {};
    
        void SetupViewportState() {
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;
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
    };