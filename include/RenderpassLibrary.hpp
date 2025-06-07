#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "RenderPassManager.hpp"
class RenderPassManager;

class RenderpassLibrary{

    public:
        RenderpassLibrary() = default;
        RenderpassLibrary(const RenderpassLibrary&) = delete;
        RenderpassLibrary& operator=(const RenderpassLibrary&) = delete;

        void AddRenderpassManager(std::string name, RenderPassManager* renderpassManager) {
            renderpasses[name] = renderpassManager; 
        }

        RenderPassManager* GetRenderpassManager(const std::string& name) {
            return renderpasses.at(name);
        }

        void Cleanup(){
            for(auto& [key,val] : renderpasses){
                val->Cleanup();
                delete(val);  //renderpass managers are allocated with new
            }
        }
    protected:
        std::unordered_map<std::string, RenderPassManager*> renderpasses;
     

};