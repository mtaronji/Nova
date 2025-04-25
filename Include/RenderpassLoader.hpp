
#pragma once
#include <vulkan/vulkan.h>
#include "json.hpp"
#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <fstream>

class RenderPassLoader {
public:
    static  VkRenderPassCreateInfo LoadFromFile(const std::string& filePath);
};