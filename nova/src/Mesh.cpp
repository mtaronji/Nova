#include "Mesh.hpp"

std::vector<VertexPC> Mesh::GenerateCubeVertices(bool solidColor) {
   
    auto randColor = []() -> glm::vec4 {
        return glm::vec4(
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            1.0f
        );
    };

    glm::vec4 colorForAll = randColor(); // used if solidColor == true

    auto getColor = [&]() -> glm::vec4 {
        return solidColor ? colorForAll : randColor();
    };

    return {
        // Front face
        {{-0.5f, -0.5f,  0.5f}, getColor()}, // 0
        {{ 0.5f, -0.5f,  0.5f}, getColor()}, // 1
        {{ 0.5f,  0.5f,  0.5f}, getColor()}, // 2
        {{-0.5f,  0.5f,  0.5f}, getColor()}, // 3

        // Back face
        {{-0.5f, -0.5f, -0.5f}, getColor()}, // 4
        {{ 0.5f, -0.5f, -0.5f}, getColor()}, // 5
        {{ 0.5f,  0.5f, -0.5f}, getColor()}, // 6
        {{-0.5f,  0.5f, -0.5f}, getColor()}, // 7
    };
}

std::vector<uint16_t> Mesh::GetCubeIndices() {
    return {
        // Front face
        0, 1, 2, 2, 3, 0,

        // Right face
        1, 5, 6, 6, 2, 1,

        // Back face
        5, 4, 7, 7, 6, 5,

        // Left face
        4, 0, 3, 3, 7, 4,

        // Top face
        3, 2, 6, 6, 7, 3,

        // Bottom face
        4, 5, 1, 1, 0, 4
    };
}