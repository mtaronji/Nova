#pragma once
#include "Shell.hpp"
#include "Renderer.hpp"
#include "VulkanEngine.hpp"

class Magusv0{
    public:
        Magusv0(const Renderer &renderer, const Shell &shell);
    
    private:
        VulkanEngine engine;
        

};