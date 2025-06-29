#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include "DescriptorsetLoader.hpp"

class CommandManager;
class BufferResource;
class Mesh;
class GPU;
class BufferOps;
class PipelineLibrary;
class ResourceManager{

    public:

        ~ResourceManager(){
            
        }
        ResourceManager(std::shared_ptr<GPU>);
        

        void SetMesh(std::string key, Mesh* mesh);
        Mesh* GetMesh(std::string key);
        void SetMeshes(std::unordered_map<std::string, Mesh*>& meshes);

        std::unordered_map<std::string, Mesh*>& GetMeshes();
        void SetDescriptorSets(std::string pipelineKey, std::vector<std::vector<BufferResource*>> descriptorSets);

        std::vector<BufferResource*>& GetDescriptorSet(std::string pipelineKey, uint32_t set);

        std::vector<std::vector<BufferResource*>>& GetDescriptorSets(std::string pipelineKey);

        void SetResourceMap(std::unordered_map<std::string, BufferResource*>& resourceMap);
        std::unordered_map<std::string, BufferResource*>& GetResourceMap();

        BufferResource* GetResource(std::string key);

        void SetResource(std::string key, BufferResource* r);

        void SetDescriptorFiles(std::unordered_map<std::string, DescriptorFile>& descriptorFiles) {
            this->descriptorFiles = descriptorFiles;
        }

        //this fills the descriptor map below. We aredn't creating new resources, just another way to access them
        //this creates a map to access descriptor set resources by set and binding and pipeline based on the names specified in the pipeline config
        //for ex - if a resource named "camera" is specified in the descriptor set, it expects that resource in the resource map                                                
        void InitializeDescriptorSetsResources(std::shared_ptr<PipelineLibrary> pipelineLibrary);

        //Create resources for a type of data
        //if usage isn't created with desttransfer or src transfer then you will be warned, no exception
        //if multiple flags are specified (vertex, index, etc) we will create a usage with each one
        void CreateMonolith(GPU* gpu, CommandManager* commandManager, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkDeviceSize initialSize);
        

        //assigning a monolith buffer will copy the data to a specified monolith buffer
        virtual void AssignMonolithBuffer(BufferResource* resource, GPU* gpu, CommandManager* cm, VkMemoryPropertyFlags memoryProperties, VkDeviceSize alignment);

        //Will try to allocate, if it's too much data, we will try to resize the buffer
        //we will allocate meshes per frame
        //if you don't wish this, create buffer resources with 1 frame
        //virtual void AssignMonolithBuffer(Mesh * m, GPU* gpu, CommandManager* cm, VkDeviceSize alignment);
        virtual void UpdateBufferData(BufferResource* resource, GPU* gpu, CommandManager* cm, uint32_t frame);

        //updating descriptor sets is putting actual data buffer location into the descriptor sets
        //pipeline key for the pipeline the descriptorset is a part of
        //the layout already created and filled
        //the descriptor set already allocated
        //the pipeline descriptorbindings specified in the config
        //the set index from the pipeline 
        //the copyIndex. When buffer resources are created, they are created with a copy parameter
        //the copy parameter allocates extra buffer space for buffer resources
        //this is used for when you want to update the resource in another frame, but can't because the frame might be in flight
        virtual void UpdateDescriptorSet(std::string pipelineKey,
                                          VkDescriptorSet descriptorSet, 
                                          VkDescriptorSetLayout descriptorLayout,
                                          std::vector<VkDescriptorSetLayoutBinding> pipelineDescriptorBindings,
                                          uint32_t setIndex,
                                          uint32_t copyIndex);
     
        void Cleanup(GPU * gpu);

    protected:
        struct Monolith{
            VkBuffer buffer = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
            VkDeviceSize memoryAllocated;
            VkDeviceSize capacity;
        };

        std::unordered_map<VkBufferUsageFlags, Monolith> monoliths = {};
        std::unordered_map<std::string, Mesh*> meshes = {};

        //all resources of the app that are descriptor sets. This pulls data straight from resource map
        //it's indexed as [set][binding] so each pipelinedescriptorsets is a 2d array of buffer resources
        std::unordered_map<std::string, std::vector<std::vector<BufferResource*>>> pipelineDescriptorSets = {}; 
        std::unordered_map<std::string, BufferResource*> resourceMap = {}; // all resources of the app

        std::unordered_map<std::string, DescriptorFile> descriptorFiles; //descriptor files from the descriptorsets files

        VkBuffer tempBuffer = VK_NULL_HANDLE;     //for copying the old buffer
        VkDeviceMemory tempMemory = VK_NULL_HANDLE;

        std::shared_ptr<GPU> gpu;
};