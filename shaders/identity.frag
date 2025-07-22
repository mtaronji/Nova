#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set = 1, binding = 0) uniform sampler2DArray texSampler;

layout(location = 0) in vec4 fragColorIn;
layout(location = 1) in vec3 fragNormalIn;
layout(location = 2) in vec3 fragTexCoordIn; // Changed to vec3 — needed for sampler2DArray

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoordIn);
    //outColor = fragColorIn;
}