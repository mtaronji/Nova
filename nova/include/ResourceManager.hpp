#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include "DescriptorsetLoader.hpp"
#include "Mesh.hpp"
#include "Descriptor.hpp"
#include "BufferResource.hpp"
#include "ImageResource.hpp"
using namespace std;
class CommandManager;
class GPU;
class PipelineLibrary;
class ResourceManager{

    public:

        ~ResourceManager(){
            
        }
        ResourceManager(shared_ptr<GPU>);
        

        void SetMesh(string key, Mesh&& mesh);
        Mesh& GetMesh(string key);
        void SetMeshes(unordered_map<string, Mesh>&&);

        unordered_map<string, Mesh>& GetMeshes();
        void SetDescriptorSets(string pipelineKey, vector<vector<shared_ptr<Descriptor>>>&&);

        vector<shared_ptr<Descriptor>>& GetDescriptorSet(std::string pipelineKey, uint32_t set);
        
        vector<vector<shared_ptr<Descriptor>>>& GetDescriptorSets(std::string pipelineKey);

        std::shared_ptr<BufferResource> GetBufferResource(std::string key);
        void SetBufferResource(std::string key, shared_ptr<BufferResource> r);

        shared_ptr<ImageResource> GetImageResource(std::string key);
        void SetImageResource(std::string key, shared_ptr<ImageResource> r);

        unordered_map<string, shared_ptr<BufferResource>>& GetBufferDescriptors(){
            return bufferDescriptors;
        }

        unordered_map<string, shared_ptr<ImageResource>>& GetImageDescriptors(){
            return imageDescriptors;
        }

        void SetResource(std::string key, std::shared_ptr<Resource> resource);
        shared_ptr<Resource> GetResource(string key);

        void SetTexture(string key, ImageResource&& r);
        void SetDescriptorFiles(unordered_map<string, DescriptorFile>& descriptorFiles) {
            this->descriptorFiles = descriptorFiles;
        }

        //this fills the descriptor map below. We aredn't creating new resources, just another way to access them
        //this creates a map to access descriptor set resources by set and binding and pipeline based on the names specified in the pipeline config
        //for ex - if a resource named "camera" is specified in the descriptor set, it expects that resource in the resource map                                                
        void InitializeDescriptorSetsResources(shared_ptr<PipelineLibrary> pipelineLibrary);

        //Create resources for a type of data
        //if usage isn't created with desttransfer or src transfer then you will be warned, no exception
        //if multiple flags are specified (vertex, index, etc) we will create a usage with each one
        void CreateMonolith(GPU* gpu, CommandManager* commandManager, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkDeviceSize initialSize);
        

        //assigning a monolith buffer will copy the data to a specified monolith buffer
        virtual void AssignMonolithBuffer(shared_ptr<BufferResource> resource, GPU* gpu, CommandManager* cm, VkMemoryPropertyFlags memoryProperties, VkDeviceSize alignment);

        //Will try to allocate, if it's too much data, we will try to resize the buffer
        //we will allocate meshes per frame
        //if you don't wish this, create buffer resources with 1 frame
        //virtual void AssignMonolithBuffer(Mesh * m, GPU* gpu, CommandManager* cm, VkDeviceSize alignment);
        virtual void UpdateBufferData(shared_ptr<BufferResource> resource, GPU* gpu, CommandManager* cm, uint32_t frame);

        //updating descriptor sets is putting actual data buffer location into the descriptor sets
        //pipeline key for the pipeline the descriptorset is a part of
        //the layout already created and filled
        //the descriptor set already allocated
        //the pipeline descriptorbindings specified in the config
        //the set index from the pipeline 
        //the copyIndex. When buffer resources are created, they are created with a copy parameter
        //the copy parameter allocates extra buffer space for buffer resources
        //this is used for when you want to update the resource in another frame, but can't because the frame might be in flight
        virtual void UpdateDescriptorSet(string pipelineKey,
                                          VkDescriptorSet descriptorSet, 
                                          VkDescriptorSetLayout descriptorLayout,
                                          vector<VkDescriptorSetLayoutBinding> pipelineDescriptorBindings,
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

        unordered_map<VkBufferUsageFlags, Monolith> monoliths = {};
        unordered_map<std::string, Mesh> meshes = {};

        //all resources of the app that are descriptor sets. This pulls data straight from the resources below
        unordered_map<string, vector<vector<shared_ptr<Descriptor>>>> pipelineDescriptorSets = {}; 

        //all resources in image resources and buffer resources are here
        //these are not our mesh data which is stored in the map above
        //these are all the individual components of the descriptor Resources above seperated based on type
        //descriptorSets above point to these resources as they are all shared_ptr
        unordered_map<string, shared_ptr<Resource>> resources = {};
        unordered_map<string, shared_ptr<ImageResource>> imageDescriptors = {};
        unordered_map<string, shared_ptr<BufferResource>> bufferDescriptors = {};

        unordered_map<string, DescriptorFile> descriptorFiles; //descriptor files from the descriptorsets files

        VkBuffer tempBuffer = VK_NULL_HANDLE;     //for copying the old buffer
        VkDeviceMemory tempMemory = VK_NULL_HANDLE;

        shared_ptr<GPU> gpu;
};