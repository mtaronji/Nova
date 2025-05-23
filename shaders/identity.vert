#version 450

// Per-vertex position input
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
 
// Uniform Buffer Object from host code
layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
    float padding;
} camera;

// Output to fragment shader (if needed)
layout(location = 0) out vec4 fragColor;

void main() {
    vec4 worldPosition = vec4(inPosition, 1.0);

    // Transform the vertex position using view and projection matrices
    gl_Position = camera.proj * camera.view * worldPosition;

    // Pass values to fragment shader (optional)
    fragColor = inColor;
}