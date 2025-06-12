#pragma once

#include <glm/glm.hpp>

struct FrameInfo {
    glm::vec2 iResolution;
    float iTime;
    int iframe;
};

struct FrameInfoModelMatrix {
    glm::vec2 iResolution;
    float iTime;
    int iframe;
    glm::mat4 model;
};