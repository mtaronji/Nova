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
#include <memory>

class GraphicsPipelineLoader {
    public:
        GraphicsPipelineLoader() = default;
        static VkGraphicsPipelineCreateInfo LoadFromFile(std::shared_ptr<GPU> gpu, const std::string& filePath);

    protected:

  

};
