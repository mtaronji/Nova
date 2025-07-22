#version 450

// Per-vertex inputs
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inColor;

// Uniform Buffer Object from host code
layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
    float padding;
} camera;

// Outputs to fragment shader
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragTexCoord; // Must be vec3 for sampler2DArray

void main() {
    vec4 worldPosition = vec4(inPosition, 1.0);

    // Transform vertex position to clip space
    gl_Position = camera.proj * camera.view * worldPosition;

    // Pass through data
    fragColor = inColor;
    fragNormal = inNormal;

    //image resources are all arrays. If it's one image, just pass 0.0
    fragTexCoord = vec3(inTexCoord, 0.0); // Set to desired layer
}