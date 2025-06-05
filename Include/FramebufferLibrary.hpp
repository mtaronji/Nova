#pragma once

#include <memory>
#include <unordered_map>

class FramebufferGenerator;

class FramebufferLibrary {
    public:
        FramebufferLibrary() = default;
        FramebufferLibrary(const FramebufferLibrary&) = delete;
        FramebufferLibrary& operator=(const FramebufferLibrary&) = delete;
        void AddFramebufferGenerator(std::string name, FramebufferGenerator* framebufferGenerator);
    
        FramebufferGenerator* GetFramebufferGenerator(const std::string& name);
        virtual void Cleanup();
    
    protected:
        std::unordered_map<std::string, FramebufferGenerator*> framebufferGenerators;

        
};