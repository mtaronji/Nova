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

std::vector<VertexPNTC> Mesh::GenerateCubeVerticesPNTC(bool solidColor) {

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
        // Front face (+Z)
        {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, getColor(), {0.0f, 0.0f}}, // 0
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, getColor(), {1.0f, 0.0f}}, // 1
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, getColor(), {1.0f, 1.0f}}, // 2
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, getColor(), {0.0f, 1.0f}}, // 3

        // Back face (-Z)
        // Back face (-Z) - made consistent with other faces
        {{0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, getColor(), {1.0f, 0.0f}}, // 4
        {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, getColor(), {0.0f, 0.0f}}, // 5
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, getColor(), {0.0f, 1.0f}}, // 6
        {{0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, getColor(), {1.0f, 1.0f}}, // 7

        // Top face (+Y)
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, getColor(), {0.0f, 0.0f}}, // 8
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, getColor(), {1.0f, 0.0f}}, // 9
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, getColor(), {1.0f, 1.0f}}, // 10
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, getColor(), {0.0f, 1.0f}}, // 11

        // Bottom face (-Y) - made consistent UV orientation
        {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, getColor(), {0.0f, 0.0f}}, // 12
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, getColor(), {1.0f, 0.0f}}, // 13
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, getColor(), {1.0f, 1.0f}}, // 14
        {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, getColor(), {0.0f, 1.0f}}, // 15

        // Right face (+X)
        {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, getColor(), {0.0f, 0.0f}}, // 16 bottom left
        {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, getColor(), {1.0f, 0.0f}}, // 17 //bottom right
        {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, getColor(), {1.0f, 1.0f}}, // 18 //top right
        {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, getColor(), {0.0f, 1.0f}}, // 19 //top left

        // Left face (-X)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, getColor(), {0.0f, 0.0f}}, // 20  //left bottom
        {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, getColor(), {1.0f, 0.0f}}, // 21   right bottom
        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, getColor(), {1.0f, 1.0f}}, // 22  //top right
        {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, getColor(), {0.0f, 1.0f}}  // 23 //top left
    };

}

std::vector<uint16_t> Mesh::GetCubeIndicesPNTC() {
    return {
        // Front (+Z)
        0, 1, 2, 2, 3, 0,

        // Back (-Z)
        4, 5, 6, 6, 7, 4,

        // Top (+Y)
        8, 9, 10, 10, 11, 8,

        // Bottom (-Y)
        12, 13, 14, 14, 15, 12, 
  
        // Right (+X)
        16, 17, 18, 18, 19, 16,

        // Left (-X)
        20, 21, 22, 22, 23, 20
    };
}