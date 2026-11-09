#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include "FramebufferGenerator.hpp"
class FramebufferLibrary {
    public:
        FramebufferLibrary() = default;
        virtual void Cleanup();
        FramebufferLibrary(const FramebufferLibrary&) = delete;
        FramebufferLibrary& operator=(const FramebufferLibrary&) = delete;
        void AddFramebufferGenerator(std::string name, FramebufferGenerator&& framebufferGenerator);
    
        FramebufferGenerator& GetFramebufferGenerator(const std::string& name);

    
    protected:
        std::unordered_map<std::string, FramebufferGenerator> framebufferGenerators;
        
};