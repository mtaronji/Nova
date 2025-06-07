#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <unordered_map>

//The alignas ensures that the memory address for the start of the ubo is at a multiple of 16
//A breakdown would be something like this:
//start of struct is at a 16byte memory address. 
//then we jump forward 16 * 4 bytes for our view (safe because that is a multiple of 16)
//another 16 * 4 bytes we find our proj
//another 12 we find our camera position
//another 4 our padding (so we reach the end of the struct at a 16 byte boundary)

// Rule	Why
// Every vec3 or vec4 member must start at 16-byte alignment	std140
// Floats and vec2s must not cross 16-byte boundaries	std140
// Matrices must have each column aligned to 16 bytes	std140
// Structs align themselves to their largest member	std140
// Add manual padding fields if necessary	C++ compiler won't guess Vulkan layout
// ⚡ Important:
// C++ cares about address alignment — Vulkan cares about memory layout inside.
//alignas(16) says the struct will always start at a 16 byte boundary

//contingruous array meant for allocating ubo data

struct alignas(16)CameraUBO {
    glm::mat4 view =- glm::mat4(1.0f);                                  //4 *4 * 4bytes = 64 = 16 * 4             offset = 0
    glm::mat4 proj = glm::mat4(1.0f);                                   //4 * 4 bytes = 64 = 16 * 4              offset = 64
    glm::vec3 cameraPosition = glm::vec3(0.0f,0.0f, 5.0f);              //4 * 3 bytes = 12 bytes          offset = 128
    float padding;                                                      //4 bytes plus the 12 bytes above make it 16       offset = 140
};                                                                      //      offset = 144 for struct = multiple of 16

struct alignas(16) ModelUBO {
    glm::mat4 model = glm::mat4(1.0f);
};

struct alignas(16) LightUBO {
    glm::vec3 lightDirection;
    float intensity;         // could also be used for attenuation
    glm::vec3 lightColor;
    float padding2;          // pad to 16 bytes again
};

struct alignas(16) MaterialUBO {
    glm::vec4 baseColor;    // vec4 to match alignment
    float roughness;
    float metallic;
    float padding[2];      // pad to 16-byte boundary
};

struct alignas(16) TimeUBO {
    float time;
    float deltaTime;
    int frame;
    float padding; // pad to 16 bytes
};


//descriptor set stuff


