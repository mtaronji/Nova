#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>
#include <array>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <cstring>
#include "GPU.hpp"

//a binding is another way of saying a memory allocation or data stream
//if you set everything in the struct below for one data stream, if pos changes and needs updating, you have to update color again
//much more efficient to have memory bindings for all the pieces of the vertex indivisually

//remember all this data is specifying how data is coming into the pipeline into the vertex shader. 
//for example, with this vertex data
//Vertex vertices[] = {
//     // Position           // Normal           // Color
//     { glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) },
//     { glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) },
//     { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) }
// };

//I can handle it a few ways. 
//1st way is to create a struct with a vec3 called position, a vec3 called normal and a vec4 called color
//then I can keep it with one binding and in the attribute part create for locations from the 1 binding.
//another way is to create 3 bindings. 1 for position, 1 for normal and 1 for color
//What you cannot do is have 1 binding and reusue the same binding
//remember, we want to send 

//VkVertexInputBindingDescription bindingDescription{};
//         bindingDescription.binding = 0;
//         bindingDescription.stride = sizeof(Vertex);
//         bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

//         return bindingDescription;
//     }

//     static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
//         std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

//         attributeDescriptions[0].binding = 0;
//         attributeDescriptions[0].location = 0;
//         attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
//         attributeDescriptions[0].offset = offsetof(Vertex, pos);

//         attributeDescriptions[1].binding = 0;
//         attributeDescriptions[1].location = 1;
//         attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
//         attributeDescriptions[1].offset = offsetof(Vertex, color);

    
struct alignas(16) VertexP{

    glm::vec3 pos;
    static uint32_t GetFieldOffset(int location){
        if(location == 0){
            return offsetof(VertexP, pos);
        }
        throw std::runtime_error("Incorrect Location specified for Vertexp : " + location);
    }
};
struct alignas(16) VertexPC{

    glm::vec3 pos;
    glm::vec4 color;
    static uint32_t GetFieldOffset(int location){
        if(location == 0) return offsetof(VertexPC,pos); 
        if(location == 1) return offsetof(VertexPC,color);

        throw std::runtime_error("Incorrect Location specified for Vertexpc : " + location);
    }
};
struct alignas(16) VertexPN{
    
    glm::vec3 pos;
    glm::vec3 normal;
    static uint32_t GetFieldOffset(int location){
        if(location == 0) return offsetof(VertexPN,pos); 
        if(location == 1) return offsetof(VertexPN,normal);
        throw std::runtime_error("Incorrect Location specified for Vertexpn: " + location);
    }
};
struct alignas(16) VertexPNT{
    
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
    static uint32_t GetFieldOffset(int location){
        if(location == 0) return offsetof(VertexPNT,pos); 
        if(location == 1) return offsetof(VertexPNT,normal);
        if(location == 2) return offsetof(VertexPNT,texCoord);
        throw std::runtime_error("Incorrect Location specified for Vertexpnt : " + location);
    }
};

struct alignas(16) VertexPT{

    glm::vec3 pos;
    glm::vec2 texCoord;
    static uint32_t GetFieldOffset(int location){
        if(location == 0) return offsetof(VertexPT,pos); 
        if(location == 1) return offsetof(VertexPT,texCoord);
        throw std::runtime_error("Incorrect Location specified for Vertexpt : " + location);
    }
};

struct alignas(16) VertexPNTC{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec4 color;
    static uint32_t GetFieldOffset(int location){
        if(location == 0) return offsetof(VertexPNTC,pos); if(location == 1) return offsetof(VertexPNTC,normal);
        if(location == 2) return offsetof(VertexPNTC,texCoord);if(location == 3) return offsetof(VertexPNTC,color);
        throw std::runtime_error("Incorrect Location specified for Vertexpntc : " + location);
    }
};



