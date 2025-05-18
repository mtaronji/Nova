#version 450

// Per-vertex position input
layout(location = 0) in vec3 inPosition;

// Uniform Buffer Object from host code
layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
    float padding;
} camera;

// Output to fragment shader (if needed)
layout(location = 0) out vec3 fragWorldPosition;
layout(location = 1) out vec3 fragViewPosition;

void main() {
    vec4 worldPosition = vec4(inPosition, 1.0);

    // Transform the vertex position using view and projection matrices
    gl_Position = camera.proj * camera.view * worldPosition;

    // Pass values to fragment shader (optional)
    fragWorldPosition = inPosition;
    fragViewPosition = (camera.view * worldPosition).xyz;
}