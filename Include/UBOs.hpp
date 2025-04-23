#pragma once

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


//descriptor set stuff


