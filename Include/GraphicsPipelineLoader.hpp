// GraphicsPipelineLoader.hpp
#pragma once

#include <vulkan/vulkan.h>
#include "json.hpp"
#include <vector>
#include <string>
#include <optional>
#include <fstream>
#include "json.hpp"
#include "Shader.hpp"
#include "GPU.hpp"

class GraphicsPipelineLoader {
    public:
        GraphicsPipelineLoader() = delete;
        GraphicsPipelineLoader();
        static VkGraphicsPipelineCreateInfo LoadFromFile(GPU& gpu, const std::string& filePath);

    protected:

  

};
