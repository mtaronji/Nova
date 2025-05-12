#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include "BufferOps.hpp"

class CommandManager;

struct BoundingBox {
    glm::vec3 min;  // Minimum corner (lowest x, y, z)
    glm::vec3 max;  // Maximum corner (highest x, y, z)

    BoundingBox() 
        : min(glm::vec3(FLT_MAX)), max(glm::vec3(-FLT_MAX)) {}

    BoundingBox(const glm::vec3& minPoint, const glm::vec3& maxPoint)
        : min(minPoint), max(maxPoint) {}

    // Expand bounding box to include a point
    void Expand(const glm::vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }

    // Transform the bounding box with a matrix
    BoundingBox Transform(const glm::mat4& matrix) const {
        glm::vec3 corners[8] = {
            {min.x, min.y, min.z}, {max.x, min.y, min.z},
            {min.x, max.y, min.z}, {max.x, max.y, min.z},
            {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {min.x, max.y, max.z}, {max.x, max.y, max.z}
        };

        BoundingBox result;
        for (int i = 0; i < 8; ++i) {
            glm::vec4 transformed = matrix * glm::vec4(corners[i], 1.0f);
            result.Expand(glm::vec3(transformed));
        }
        return result;
    }

    // Center point of the box
    glm::vec3 Center() const {
        return (min + max) * 0.5f;
    }

    // Size (extent) of the box
    glm::vec3 Extents() const {
        return max - min;
    }
};

struct Mesh {
    Mesh() = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(BufferResource* vertices, BufferResource* indices){this->vertexResource = vertices; this->indiceResource = indices;}
    ~Mesh(){
       
    }
    void Cleanup(GPU *gpu){
        if(vertexResource != nullptr){vertexResource->Cleanup(gpu); vertexResource = nullptr;}
        if(indiceResource != nullptr){indiceResource->Cleanup(gpu); indiceResource = nullptr;}
    }

    void CreateGPUResources(GPU* gpu, CommandManager* commandManager){
        BufferOps::CreateOnGPUBuffer(*gpu, *commandManager, vertexResource->GetInitialDataP(), vertexResource->GetDataSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexResource->GetBuffer(), vertexResource->GetMemory());
        BufferOps::CreateOnGPUBuffer(*gpu, *commandManager, indiceResource->GetInitialDataP(), indiceResource->GetDataSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indiceResource->GetBuffer(), indiceResource->GetMemory());
    }
    glm::mat4 modelMatrix;
    BoundingBox aabb;
    BufferResource* vertexResource = nullptr;
    BufferResource* indiceResource = nullptr;
    bool visible = true;
};


struct alignas(16) VertexP {
    glm::vec3 pos;

    static uint32_t GetFieldOffset(int location) {
        if (location == 0) return offsetof(VertexP, pos);
        throw std::runtime_error("Incorrect Location specified for VertexP: " + std::to_string(location));
    }

    static VkVertexInputBindingDescription GetBindingDescription() {
        return {0, sizeof(VertexP), VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
        return {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, GetFieldOffset(0)}
        };
    }
};

// Vertex with position + color
struct alignas(16) VertexPC {
    glm::vec3 pos;
    float _padding = 0.0f;
    glm::vec4 color;
    VertexPC(glm::vec3 p, glm::vec4 c) : pos(p), _padding(0.0f), color(c) {}

    static uint32_t GetFieldOffset(int location) {
        if (location == 0) return offsetof(VertexPC, pos);
        if (location == 1) return offsetof(VertexPC, color);
        throw std::runtime_error("Incorrect Location specified for VertexPC: " + std::to_string(location));
    }

    static VkVertexInputBindingDescription GetBindingDescription() {
        return {0, sizeof(VertexPC), VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
        VkVertexInputAttributeDescription vd;
        
        return {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPC, pos)},     // pos at offset 0
            {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(VertexPC, color)} // color at offset 16
        };
    }
};

// Vertex with position + normal
struct alignas(16) VertexPN {
    glm::vec3 pos;
    float _padding = 0.0f;
    glm::vec3 normal;
    
    VertexPN(glm::vec3 p, glm::vec3 n) : pos(p), _padding(0.0f), normal(n) {}

    static uint32_t GetFieldOffset(int location) {
        if (location == 0) return offsetof(VertexPN, pos);
        if (location == 1) return offsetof(VertexPN, normal);
        throw std::runtime_error("Incorrect Location specified for VertexPC: " + std::to_string(location));
    }

    static VkVertexInputBindingDescription GetBindingDescription() {
        return {0, sizeof(VertexPN), VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
        return {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPN, pos)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPN, normal)}
        };
    }
};

// Vertex with position + normal + texCoord
struct alignas(16) VertexPNT {
    glm::vec3 pos;
    float _padding1 = 0.0f;
    glm::vec3 normal;
    float _padding2 = 0.0f;
    glm::vec2 texCoord;
    VertexPNT(glm::vec3 p, glm::vec3 n, glm::vec2 tc) : pos(p), _padding1(0.0f), _padding2(0.0f), normal(n), texCoord(tc) {}

    static uint32_t GetFieldOffset(int location) {
        if (location == 0) return offsetof(VertexPNT, pos);
        if (location == 1) return offsetof(VertexPNT, normal);
        if (location == 2) return offsetof(VertexPNT, texCoord);
        throw std::runtime_error("Incorrect Location specified for VertexPC: " + std::to_string(location));
    }

    static VkVertexInputBindingDescription GetBindingDescription() {
        return {0, sizeof(VertexPNT), VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
        return {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPNT, pos)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPNT, normal)},
            {2, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(VertexPNT, texCoord)}
        };
    }
};

// Vertex with position + texCoord
struct alignas(16) VertexPT {
    glm::vec3 pos;
    float _padding = 0.0f;
    glm::vec2 texCoord;
    VertexPT(glm::vec3 p, glm::vec2 tc ) : pos(p), _padding(0.0f), texCoord(tc) {}
    static uint32_t GetFieldOffset(int location) {
        if (location == 0) return offsetof(VertexPT, pos);
        if (location == 1) return offsetof(VertexPT, texCoord);
        throw std::runtime_error("Incorrect Location specified for VertexPC: " + std::to_string(location));
    }
    static VkVertexInputBindingDescription GetBindingDescription() {
        return {0, sizeof(VertexPT), VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
        return {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPT, pos)},
            {1, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(VertexPT, texCoord)}
        };
    }
};

// Vertex with position + normal + texCoord + color
struct alignas(16) VertexPNTC {
    glm::vec3 pos;
    float _padding1 = 0.0f;
    glm::vec3 normal;
    float _padding2 = 0.0f;
    glm::vec2 texCoord;
    float _padding3[2] = {0.0f,0.0f};
    glm::vec4 color;

    VertexPNTC(glm::vec3 p, glm::vec3 n, glm::vec4 c, glm::vec2 tc ) : pos(p), normal(n), texCoord(tc), color(c), _padding1(0.0f), _padding2(0.0f), _padding3{0.0f,0.0f} {}
    static uint32_t GetFieldOffset(int location) {
        if (location == 0) return offsetof(VertexPNTC, pos);
        if (location == 1) return offsetof(VertexPNTC, normal);
        if (location == 2) return offsetof(VertexPNTC, texCoord);
        if (location == 3) return offsetof(VertexPNTC, color);
        throw std::runtime_error("Incorrect Location specified for VertexPC: " + std::to_string(location));
    }

    static VkVertexInputBindingDescription GetBindingDescription() {
        return {0, sizeof(VertexPNTC), VK_VERTEX_INPUT_RATE_VERTEX};
    }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
        return {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT,    offsetof(VertexPNTC, pos)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT,    offsetof(VertexPNTC, normal)},
            {2, 0, VK_FORMAT_R32G32_SFLOAT,       offsetof(VertexPNTC, texCoord)},
            {3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(VertexPNTC, color)}
        };
    }
};

