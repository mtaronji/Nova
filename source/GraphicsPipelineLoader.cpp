#include "GraphicsPipelineLoader.hpp"
#include "PushConstants.hpp"
#include "Shader.hpp"
#include "GPU.hpp"
#include "Mesh.hpp"

using json = nlohmann::json;



static VkPolygonMode ParsePolygonMode(const std::string str){
    if(str == "fill") return VK_POLYGON_MODE_FILL;
    if(str == "line") return VK_POLYGON_MODE_LINE;
    if(str == "point") return VK_POLYGON_MODE_POINT;
    throw std::runtime_error("Failed to find polygon mode in Graphics Config  for: " + str);

}

static VkCullModeFlags ParseCullMode(const std::string str){
    if(str == "none") return VK_CULL_MODE_NONE;
    if(str == "front") return VK_CULL_MODE_FRONT_BIT;
    if(str == "back") return VK_CULL_MODE_BACK_BIT;
    if(str == "front_and_back") return VK_CULL_MODE_FRONT_AND_BACK;
    throw std::runtime_error("Failed to find cull mode  in Graphics Config for: " + str);
}


static VkFrontFace ParseFrontFace(const std::string str){
    if(str == "clockwise") return VK_FRONT_FACE_CLOCKWISE;
    if(str == "counter_clockwise") return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    throw std::runtime_error("Failed to find Front face settings in Graphics Config  for: " + str);
}

static VkSampleCountFlagBits ParseRasterizationSamples(const int number){
    if(number == 1) return VK_SAMPLE_COUNT_1_BIT;
    if(number == 2) return VK_SAMPLE_COUNT_2_BIT;
    if(number == 4) return VK_SAMPLE_COUNT_4_BIT;
    if(number == 8) return VK_SAMPLE_COUNT_8_BIT;
    if(number == 16) return VK_SAMPLE_COUNT_16_BIT;
    if(number == 32) return VK_SAMPLE_COUNT_32_BIT;
    if(number == 64) return VK_SAMPLE_COUNT_64_BIT;

    throw std::runtime_error("Unsupported Sample count: in Graphics Config  " + number);

}

static VkDynamicState ParseDynamicState(const std::string str){
    if(str == "viewport") return VK_DYNAMIC_STATE_VIEWPORT;
    if(str == "scissor") return VK_DYNAMIC_STATE_SCISSOR;
    if(str == "lineWidth") return VK_DYNAMIC_STATE_LINE_WIDTH;
    if(str == "depthBias") return VK_DYNAMIC_STATE_DEPTH_BIAS;	
    if(str == "blendConstants") return VK_DYNAMIC_STATE_BLEND_CONSTANTS;	
    if(str == "depthBounds") return VK_DYNAMIC_STATE_DEPTH_BOUNDS;	
    if(str == "stencilCompareMask") return VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;	
    if(str == "stencilWriteMask") return VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;	
    if(str == "stencilReference") return VK_DYNAMIC_STATE_STENCIL_REFERENCE;
    throw std::runtime_error("Unsupported dynamic state:  in Graphics Config  " + str);
}


static VkLogicOp ParseLogicOp(const std::string str){
    if (str == "VK_LOGIC_OP_CLEAR") return VK_LOGIC_OP_CLEAR;
    else if (str == "VK_LOGIC_OP_AND") return VK_LOGIC_OP_AND;
    else if (str == "VK_LOGIC_OP_AND_REVERSE") return VK_LOGIC_OP_AND_REVERSE;
    else if (str == "VK_LOGIC_OP_COPY") return VK_LOGIC_OP_COPY;
    else if (str == "VK_LOGIC_OP_AND_INVERTED") return VK_LOGIC_OP_AND_INVERTED;
    else if (str == "VK_LOGIC_OP_NO_OP") return VK_LOGIC_OP_NO_OP;
    else if (str == "VK_LOGIC_OP_XOR") return VK_LOGIC_OP_XOR;
    else if (str == "VK_LOGIC_OP_OR") return VK_LOGIC_OP_OR;
    else if (str == "VK_LOGIC_OP_NOR") return VK_LOGIC_OP_NOR;
    else if (str == "VK_LOGIC_OP_EQUIVALENT") return VK_LOGIC_OP_EQUIVALENT;
    else if (str == "VK_LOGIC_OP_INVERT") return VK_LOGIC_OP_INVERT;
    else if (str == "VK_LOGIC_OP_OR_REVERSE") return VK_LOGIC_OP_OR_REVERSE;
    else if (str == "VK_LOGIC_OP_COPY_INVERTED") return VK_LOGIC_OP_COPY_INVERTED;
    else if (str == "VK_LOGIC_OP_OR_INVERTED") return VK_LOGIC_OP_OR_INVERTED;
    else if (str == "VK_LOGIC_OP_NAND") return VK_LOGIC_OP_NAND;
    else if (str == "VK_LOGIC_OP_SET") return VK_LOGIC_OP_SET;
    else {
        throw std::runtime_error("Unknown VkLogicOp string:  in Graphics Config " + str);
    }
}


static VkBlendFactor ParseBlendFactor(const std::string str){
    if(str == "VK_BLEND_FACTOR_ZERO") return VK_BLEND_FACTOR_ZERO;
    if(str == "VK_BLEND_FACTOR_ONE") return VK_BLEND_FACTOR_ONE;	
    if(str == "VK_BLEND_FACTOR_SRC_COLOR") return VK_BLEND_FACTOR_SRC_COLOR;	
    if(str == "VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR") return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;	
    if(str == "VK_BLEND_FACTOR_DST_COLOR") return VK_BLEND_FACTOR_DST_COLOR;	
    if(str == "VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR") return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;	
    if(str == "VK_BLEND_FACTOR_SRC_ALPHA") return VK_BLEND_FACTOR_SRC_ALPHA;	
    if(str == "VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA") return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;	
    if(str == "VK_BLEND_FACTOR_DST_ALPHA") return VK_BLEND_FACTOR_DST_ALPHA;	
    if(str == "VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA") return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;	
    if(str == "VK_BLEND_FACTOR_CONSTANT_COLOR") return VK_BLEND_FACTOR_CONSTANT_COLOR;	
    if(str == "VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR") return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;	
    if(str == "VK_BLEND_FACTOR_CONSTANT_ALPHA") return VK_BLEND_FACTOR_CONSTANT_ALPHA;	
    if(str == "VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA") return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;	
    if(str == "VK_BLEND_FACTOR_SRC_ALPHA_SATURATE") return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;	
    if(str == "VK_BLEND_FACTOR_SRC1_COLOR") return VK_BLEND_FACTOR_SRC1_COLOR;	
    if(str == "VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR") return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;	
    if(str == "VK_BLEND_FACTOR_SRC1_ALPHA") return VK_BLEND_FACTOR_SRC1_ALPHA;	
    if(str == "VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA") return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    throw std::runtime_error("Unsupported Blend Factor:  in Graphics Config " + str);
}


static VkBlendOp ParseBlendOp(const std::string str){
   
    if(str == "VK_BLEND_OP_ADD") return VK_BLEND_OP_ADD;
    if(str == "VK_BLEND_OP_SUBTRACT") return VK_BLEND_OP_SUBTRACT;	
    if(str == "VK_BLEND_OP_REVERSE_SUBTRACT") return VK_BLEND_OP_REVERSE_SUBTRACT;	
    if(str == "VK_BLEND_OP_MIN") return VK_BLEND_OP_MIN;	
    if(str == "VK_BLEND_OP_MAX") return VK_BLEND_OP_MAX;
    throw std::runtime_error("Unsupported Parse Blend Op: " + str);
}


static VkColorComponentFlags ParseColorComponents(const std::string str){
    if(str == "g") return VK_COLOR_COMPONENT_G_BIT;
    if(str == "r") return VK_COLOR_COMPONENT_R_BIT;
    if(str == "b") return VK_COLOR_COMPONENT_B_BIT;
    if(str == "a") return VK_COLOR_COMPONENT_A_BIT;
    throw std::runtime_error("Unsupported Color component Mask: " + str);
}
static std::vector<char> ReadFile(const std::string& path) {
    
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open shader file:  in Graphics Config " + path);

    size_t size = (size_t)file.tellg();
    std::vector<char> buffer(size);
    file.seekg(0);
    file.read(buffer.data(), size);
    return buffer;
}
static VkPrimitiveTopology ParseToplogy(const std::string str){
  
    if(str == "point_list") return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    if(str == "line_list") return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    if(str == "line_strip") return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    if(str == "triangle_list") return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    if(str == "triangle_strip") return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    if(str == "triangle_fan") return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    throw std::runtime_error("Unsupported toplogy: in Graphics Config  " + str);
}


static VkVertexInputRate ParseInputRate(const std::string str){
    if(str == "vertex") return VK_VERTEX_INPUT_RATE_VERTEX;
    if(str == "instance") return VK_VERTEX_INPUT_RATE_INSTANCE;
    throw std::runtime_error("Unsupported vertex input rate: " + str);
}

static uint32_t ParseBindingStride(const std::string vertexType){
    if(vertexType == "vertexp") return sizeof(VertexP);   
    if(vertexType == "vertexpc") return sizeof(VertexPC);   
    if(vertexType == "vertexpn") return sizeof(VertexPN);  
    if(vertexType == "vertexpt") return sizeof(VertexPT);  
    if(vertexType == "vertexpnt") return sizeof(VertexPNT);   
    if(vertexType == "vertexpntc") return sizeof(VertexPNTC);
    throw std::runtime_error("Unsupported binding data in Graphics Config : " + vertexType);
}


static uint32_t ParseLocationOffset(const std::string vertexType, const int location){
    
    if(vertexType == "vertexp")return VertexP::GetFieldOffset(location); 
    if(vertexType == "vertexpc")return VertexPC::GetFieldOffset(location);
    if(vertexType == "vertexpn") return VertexPN::GetFieldOffset(location);
    if(vertexType == "vertexpt") return VertexPT::GetFieldOffset(location);
    if(vertexType == "vertexpnt") return VertexPNT::GetFieldOffset(location);
    if(vertexType == "vertexpntc") return VertexPNTC::GetFieldOffset(location);
    throw std::runtime_error("Unsupported binding data: in Graphics Config  " + vertexType);
}


static VkFormat ParseAttributeDescriptionFormat(const std::string format){
    if(format == "VK_FORMAT_R32G32B32_SFLOAT") return VK_FORMAT_R32G32B32_SFLOAT;
    if(format == "VK_FORMAT_R32_SFLOAT") return VK_FORMAT_R32_SFLOAT;
    if(format == "VK_FORMAT_R32G32B32A32_SFLOAT") return VK_FORMAT_R32G32B32A32_SFLOAT;
    if(format == "VK_FORMAT_R32G32B32_SINT") return VK_FORMAT_R32G32B32_SINT;
    if(format == "VK_FORMAT_R32G32B32_UINT") return VK_FORMAT_R32G32B32_UINT;
    if(format == "VK_FORMAT_R8G8B8A8_UNORM") return VK_FORMAT_R8G8B8A8_UNORM;
                        
    throw std::runtime_error("Unsupported format for attribute description in Graphics Config  " + format);                            
}


static VkShaderStageFlags ParseShaderStage(const std::string stage){
    if("vertex") return	VK_SHADER_STAGE_VERTEX_BIT;
    if("fragment") return VK_SHADER_STAGE_FRAGMENT_BIT;
    if("geometry") return VK_SHADER_STAGE_GEOMETRY_BIT;
    if("compute") return VK_SHADER_STAGE_COMPUTE_BIT;
    if("tessellationControl") return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    if("tessellationEvaluation") return	VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    if("allGraphics") return VK_SHADER_STAGE_ALL_GRAPHICS;
    if("all") return VK_SHADER_STAGE_ALL;
    if("raygen") return	VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    if("anyHit") return	VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
    if("closestHit") return	VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    if("miss") return VK_SHADER_STAGE_MISS_BIT_KHR;
    if("intersection") return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
    if("callable") return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    if("task") return VK_SHADER_STAGE_TASK_BIT_EXT;
    if("mesh") return VK_SHADER_STAGE_MESH_BIT_EXT;

    throw std::runtime_error("Unsupported Shader stage in Graphics Config " + stage);       
}


static VkDescriptorType ParseDescriptorType(const std::string descriptorType){
    if("uniformBuffer") return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    if("uniformBufferDynamic") return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    if("storageBuffer") return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    if("storageBufferDynamic") return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    if("sampler") return VK_DESCRIPTOR_TYPE_SAMPLER;
    if("combinedImageSampler") return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    if("sampledImage") return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    if("storageImage") return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    if("uniformTexelBuffer") return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    if("storageTexelBuffer") return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    if("inputAttachment") return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    if("inlineUniformBlock") return VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK;
    if("accelerationStructure") return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    throw std::runtime_error("could not find Descriptor Type in Graphics Config: " + descriptorType);    
}


static VkCompareOp ParseCompareOp(const std::string compareOp){
  
    if(compareOp == "never"){return VK_COMPARE_OP_NEVER;}
    if(compareOp == "less"){return VK_COMPARE_OP_LESS;}
    if(compareOp == "equal"){return VK_COMPARE_OP_EQUAL;}
    if(compareOp == "less_or_equal"){return VK_COMPARE_OP_LESS_OR_EQUAL;}
    if(compareOp == "greater"){return VK_COMPARE_OP_GREATER;}
    if(compareOp == "not_equal"){return VK_COMPARE_OP_NOT_EQUAL;}
    if(compareOp == "greater_or_equal"){return VK_COMPARE_OP_GREATER_OR_EQUAL;}
    if(compareOp == "always"){return VK_COMPARE_OP_ALWAYS;}

    throw std::runtime_error("compare op string not found");
}


static size_t ParsePushConstantSize(const std::string name){
    if(name == "frameinfo"){
        return sizeof(FrameInfo);
    }
    throw std::runtime_error("push constant not supported: " + name);
}


void GraphicsPipelineLoader::LoadFromFile(
                                            const std::string& filePath,
                                            std::vector<char> & vertexShaderCodeOut,
                                            std::vector<char> & fragmentShaderCodeOut,
                                            std::vector<char> & computeShaderCodeOut,
                                            std::vector<char> & geometryShaderCodeOut,
                                            VkPipelineInputAssemblyStateCreateInfo& inputAssemblyOut,
                                            VkPipelineRasterizationStateCreateInfo& rasterizerCreateInfoOut,
                                            VkPipelineDepthStencilStateCreateInfo& depthStencilOut,
                                            VkPipelineMultisampleStateCreateInfo& multisamplingOut,
                                            VkPipelineColorBlendStateCreateInfo& colorBlendingOut,
                                            std::vector<VkPipelineColorBlendAttachmentState>& colorblendAttachmentsOut,
                                            std::vector<VkDynamicState>& dynamicStatesOut,
                                            std::vector<std::vector<VkDescriptorSetLayoutBinding>>& descriptorSetsOut,
                                            std::unordered_map<uint32_t, std::vector<std::string>>& descriptorNamesOut,
                                            std::vector<VkPushConstantRange>& pushConstantRangesOut,
                                            std::string& renderpassKeyOut,
                                            std::string& vertexTypeOut
                                        ){

    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open graphics config: " + filePath);
    }
    json j;
    in >> j;
    
    vertexShaderCodeOut = {}; fragmentShaderCodeOut = {}; computeShaderCodeOut = {}; geometryShaderCodeOut = {};
    for (const auto& a : j["shaderStages"]) {
        
        if(a["stage"].is_string() && a["stage"] == "vertex" && a["path"].is_string()){  
            std::string path = a["path"];
            vertexShaderCodeOut = ReadFile(std::string(OUTPUT_DIRECTORY "/") + path);
        }
        if(a["stage"].is_string() && a["stage"] == "fragment"){
            std::string path = a["path"];
            fragmentShaderCodeOut = ReadFile(std::string(OUTPUT_DIRECTORY "/") + path);
        }
        if(a["stage"].is_string() && a["stage"] == "geometry"){
            std::string path = a["path"];
            geometryShaderCodeOut = ReadFile(std::string(OUTPUT_DIRECTORY "/") + path);
        }
        if(a["stage"].is_string() && a["stage"] == "computeShader"){
            std::string path = a["path"];
            computeShaderCodeOut = ReadFile(std::string(OUTPUT_DIRECTORY "/") + path);
        }   
        if(a["stage"].is_string() && a["stage"] == "tessellationControlShader"){
            std::string path = a["path"];
            ReadFile(std::string(OUTPUT_DIRECTORY "/") + path);
            
        }
        if(a["stage"].is_string() && a["stage"] == "tessellationEvaluationShader"){
            std::string path = a["path"];
            ReadFile(std::string(OUTPUT_DIRECTORY "/") + path);
            
        }
        
    }
    
    const auto& iassembly = j["inputAssembly"];
    inputAssemblyOut = {};
    inputAssemblyOut.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyOut.topology = ParseToplogy(iassembly["topology"]);
    inputAssemblyOut.primitiveRestartEnable = iassembly["primitiveRestartEnable"];
    

    // for (const auto& a : j["viewport"]) {
        
    // }
    // for (const auto& a : j["scissor"]) {
        
    // }

    const auto &r = j["rasterizer"];
    rasterizerCreateInfoOut = {};
    rasterizerCreateInfoOut.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    if (r.contains("depthClampEnable"))rasterizerCreateInfoOut.depthBiasClamp = r["depthClampEnable"];
   
    rasterizerCreateInfoOut.depthBiasEnable = r["depthBiasEnable"];
    rasterizerCreateInfoOut.rasterizerDiscardEnable = r["rasterizerDiscardEnable"];
    rasterizerCreateInfoOut.polygonMode = ParsePolygonMode(r["polygonMode"]);
    rasterizerCreateInfoOut.lineWidth = r["lineWidth"];
    rasterizerCreateInfoOut.cullMode = ParseCullMode(r["cullMode"]);
    
    const auto& m = j["multisampling"]; 
    multisamplingOut = {};
    multisamplingOut.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisamplingOut.rasterizationSamples = ParseRasterizationSamples(m["rasterizationSamples"]);
    multisamplingOut.sampleShadingEnable = m["sampleShadingEnable"];
    
    renderpassKeyOut = j["renderpasskey"];

    vertexTypeOut = j["vertextype"];
    
    depthStencilOut = {};

    if(j.contains("depthStencil")){
        depthStencilOut.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        const auto& a = j["depthStencil"];
        depthStencilOut.depthTestEnable = a["depthTestEnable"];
        depthStencilOut.depthWriteEnable = a["depthWriteEnable"];
        depthStencilOut.depthCompareOp = ParseCompareOp(a["depthCompareOp"]);
        depthStencilOut.stencilTestEnable = a["stencilTestEnable"];
        
    }
    else{
        depthStencilOut.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilOut.depthTestEnable = VK_FALSE;
        depthStencilOut.depthWriteEnable = VK_FALSE;
        depthStencilOut.depthCompareOp = VK_COMPARE_OP_ALWAYS;
        depthStencilOut.stencilTestEnable = VK_FALSE;
    }

    const auto& cb = j["colorBlend"];
    colorBlendingOut = {};
    colorBlendingOut.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorblendAttachmentsOut = {};

    colorBlendingOut.logicOpEnable = cb["logicOpEnable"];

    if(cb.contains("logicOp") && colorBlendingOut.logicOpEnable){
        colorBlendingOut.logicOp = ParseLogicOp(cb["logicOp"]);
    }
    
  
    int index = 0;
    for(const auto& b: cb["blendConstants"]){
        colorBlendingOut.blendConstants[index] = b;
        index++;
    }
    
    for (const auto& attachmentInfo : cb["attachments"]) {
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        
        colorBlendAttachment.blendEnable = attachmentInfo["blendEnable"];

        if(colorBlendAttachment.blendEnable){

            if(attachmentInfo.contains("alphaBlendOp") && attachmentInfo.contains("srcAlphaBlendFactor") && attachmentInfo.contains("dstAlphaBlendFactor") ){
                colorBlendAttachment.alphaBlendOp = ParseBlendOp(attachmentInfo["alphaBlendOp"]);
                colorBlendAttachment.dstAlphaBlendFactor = ParseBlendFactor(attachmentInfo["dstAlphaBlendFactor"]);
                colorBlendAttachment.srcAlphaBlendFactor = ParseBlendFactor(attachmentInfo["srcAlphaBlendFactor"]);
            }
            if(attachmentInfo.contains("colorBlendOp") && attachmentInfo.contains("srcColorBlendFactor") && attachmentInfo.contains("dstColorBlendFactor") ){
                colorBlendAttachment.dstColorBlendFactor = ParseBlendFactor(attachmentInfo["dstColorBlendFactor"]);
                colorBlendAttachment.srcColorBlendFactor = ParseBlendFactor(attachmentInfo["srcColorBlendFactor"]);
                colorBlendAttachment.colorBlendOp = ParseBlendOp(attachmentInfo["colorBlendOp"]);
            }

        }
        colorBlendAttachment.colorWriteMask = 0;
        for(const auto& mask : attachmentInfo["colorWriteMask"]){
            colorBlendAttachment.colorWriteMask |= ParseColorComponents(mask);
        }
        colorblendAttachmentsOut.push_back(colorBlendAttachment);
    }  
    colorBlendingOut.attachmentCount = colorblendAttachmentsOut.size();
    colorBlendingOut.pAttachments = colorblendAttachmentsOut.data();           
    
    dynamicStatesOut = {};
    if(j.contains("dynamicStates")){
        for (const auto& a : j["dynamicStates"]) {
            dynamicStatesOut.push_back(ParseDynamicState(a));
        }
    }
    
    // VkPipelineDynamicStateCreateInfo dynamicState{};
    // dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    // dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    // dynamicState.pDynamicStates = dynamicStates.data();

    
    if(j.contains("pushConstants")){
        for(const auto& pc : j["pushConstants"]){
            VkPushConstantRange pcr = {};
            pcr.size = ParsePushConstantSize(pc["name"]);

            for(const auto& s : pc["stage"]){
                 pcr.stageFlags |= ParseShaderStage(s);
            }

            pcr.offset = pc["offset"];
            pushConstantRangesOut.push_back(pcr);
        } 

    }
    if(j.contains("descriptorSets")){

        uint32_t set = 0;
        
        for(const auto& layout : j["descriptorSets"]){
            
            std::vector<VkDescriptorSetLayoutBinding> descSetLayoutBindings ={};
            uint32_t binding = 0;
            std::vector<std::string> descriptorNames;
            for (const auto& dsl : layout["descriptorSetLayout"]) {
                
                VkDescriptorSetLayoutBinding desclayoutbinding{};
                desclayoutbinding.binding = binding;
                desclayoutbinding.descriptorCount = dsl["descriptorCount"];
                desclayoutbinding.stageFlags = 0;
                for(const auto& stage : dsl["shaderStage"]){
                    desclayoutbinding.stageFlags |= ParseShaderStage(stage);
                }

                desclayoutbinding.descriptorType = ParseDescriptorType(dsl["descriptorType"]);        
                descriptorNames.push_back(dsl["name"]);  
                descSetLayoutBindings.push_back(desclayoutbinding);
                binding++;
            }
            descriptorSetsOut.push_back(descSetLayoutBindings);
            descriptorNamesOut[set] = descriptorNames;
            set++;
        
        }
    }
    
}

