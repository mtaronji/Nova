#pragma once
#include <unordered_map>
#include "BufferOps.hpp"
#include <string>
#include <stdexcept>
#include "Mesh.hpp"
#include "GPU.hpp"

class ResourceManager{

    public:
        ~ResourceManager(){
            
        }
        ResourceManager() = delete;
        ResourceManager(GPU* gpu): gpu(gpu){
            uboBuffers =  new std::unordered_map<std::string, BufferResource*>();
            cppResources = new std::unordered_map<std::string, void*>();
            meshes = new std::unordered_map<std::string, Mesh*>();
            descriptorSetBuffers = new std::vector<std::vector<BufferResource*>>();
        
        }

        void RemoveUBOResource(std::string key){
            if(uboBuffers->contains(key)){
                auto resource = (*uboBuffers)[key];
                resource->Cleanup(gpu);
                (*uboBuffers).erase(key);
            }

        }
        void AddMesh(std::string key, Mesh* mesh){
            (*meshes)[key] = mesh;
        }
        void UpdateCPPResource(std::string key, void* object ){      
            (*cppResources)[key] = object;
        }

        void AddUBOResource(std::string key, BufferResource* resource){
            RemoveUBOResource(key);
            (*uboBuffers)[key] = resource;
        }        

        void AddDescriptorSetBuffers(std::vector<BufferResource*> descriptorSetBuffers){
            this->descriptorSetBuffers->push_back(descriptorSetBuffers);
        }

        std::vector<BufferResource*>& ReceiveDescriptorSetBuffers(uint32_t set){
            return descriptorSetBuffers->at(set);           
        }
        BufferResource* RetrieveVulkanResource(std::string key){
            return uboBuffers->at(key);
        }

        void* RetrieveCPPResource(std::string key){
            return (*cppResources)[key];
        }

        Mesh* RetrieveMesh(std::string key){
            return (*meshes)[key];
        }
        std::unordered_map<std::string, BufferResource*>* RetreiveAllUBOBuffers(){
            return uboBuffers;
        }

        std::unordered_map<std::string, void*>* RetreiveAllCPPResources(){
            return cppResources;
        }
        std::unordered_map<std::string, Mesh*>* RetreiveAllMeshes(){
            return meshes;
        }

        void CreateGPUResources(GPU* gpu, CommandManager* commandManager){
            for(auto& [key,value] : *meshes){
                value->CreateGPUResources(gpu,commandManager);
            }
        }


        void Cleanup(){
            for(auto& [key,value] : *uboBuffers){
                value->Cleanup(gpu);
            }
            if(uboBuffers != nullptr){ 
                 delete(uboBuffers);
                 uboBuffers = nullptr;
            }
            if(descriptorSetBuffers != nullptr){
                delete(descriptorSetBuffers);
                descriptorSetBuffers = nullptr;
            }
            if(meshes != nullptr){
                delete(meshes);
                meshes = nullptr;
            }
            if(cppResources != nullptr){
                delete(cppResources);
                cppResources = nullptr;
            }          
           
        }

        
    protected:

        std::unordered_map<std::string, Mesh*>* meshes = nullptr;
        std::unordered_map<std::string, void*>* cppResources = nullptr;
        std::unordered_map<std::string, BufferResource*> *uboBuffers = nullptr;
        std::vector<std::vector<BufferResource*>> *descriptorSetBuffers = nullptr; 
        GPU * gpu;
};