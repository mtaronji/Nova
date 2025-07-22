#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include <string>

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
        return {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPC, pos)},     // pos at offset 0      //location 0
            {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(VertexPC, color)} // color at offset 16   //location 1
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
        throw std::runtime_error("Incorrect Location specified for VertexPNT: " + std::to_string(location));
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
        throw std::runtime_error("Incorrect Location specified for VertexPT: " + std::to_string(location));
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

        assert(false);
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
