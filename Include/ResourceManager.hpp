#pragma once
#include <unordered_map>
#include "BufferOps.hpp"
#include <string>
#include <stdexcept>
#include "Mesh.hpp"
#include "GPU.hpp"
#include "BufferOps.hpp"
#include "BufferResource.hpp"
#include <iostream>

class ResourceManager{

    public:

        ~ResourceManager(){
            
        }
        ResourceManager(){
        
        }

        void SetMesh(std::string key, Mesh* mesh){
            meshes[key] = mesh;
        }   
        Mesh* GetMesh(std::string key){
            return meshes[key];
        }
        void SetMeshes(std::unordered_map<std::string, Mesh*>& meshes){
            this->meshes = meshes;
        }
        std::unordered_map<std::string, Mesh*>& GetMeshes(){
            return meshes;
        }
        void SetDescriptorSets(std::vector<std::vector<BufferResource*>>& descriptorSets){
            this->descriptorSets = descriptorSets;
        }

        std::vector<BufferResource*>& GetDescriptorSet(uint32_t set){
            return descriptorSets[set];         
        }    

        std::vector<std::vector<BufferResource*>>& GetDescriptorSets(){
            return descriptorSets;
        }

        void SetResourceMap(std::unordered_map<std::string, BufferResource*>& resourceMap){
            this->resourceMap = resourceMap;
        }
        std::unordered_map<std::string, BufferResource*>& GetResourceMap(){
            return this->resourceMap;
        }

        BufferResource* GetResource(std::string key){
            return resourceMap[key];
        }

        void SetResource(std::string key, BufferResource* r){
            resourceMap[key] = r;
        }

        //Create resources for a type of data
        //if usage isn't created with desttransfer or src transfer then you will be warned, no exception
        //if multiple flags are specified (vertex, index, etc) we will create a usage with each one
        void CreateMonolith(GPU* gpu, CommandManager* commandManager, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkDeviceSize initialSize){

            bool hasTransferDst = usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            bool hasTransferSrc = usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            bool isUniformBuffer = usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

            VkBufferUsageFlags transfers = 0; 
            if(hasTransferDst){transfers |= VK_BUFFER_USAGE_TRANSFER_DST_BIT; }
            if(hasTransferSrc){transfers |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT; }

            if ((!hasTransferDst || !hasTransferSrc) && !isUniformBuffer) {
                std::cerr << "Warning: Creating a monolithic buffer without transfer src/dst for anything besides uniform buffers will prevent resizing or staging.\n";
            }
            
            //create a buffer monolith for each type specified         
            if(usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT){
                BufferOps::CreateBuffer(*gpu, initialSize, transfers | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryProperties, tempBuffer, tempMemory);     
                Monolith m = { .buffer = tempBuffer, .memory = tempMemory, .memoryAllocated = 0, .capacity = initialSize};
                monoliths[VK_BUFFER_USAGE_VERTEX_BUFFER_BIT] = m;
                tempBuffer = VK_NULL_HANDLE;
                tempMemory = VK_NULL_HANDLE;
            }
            if(usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT){
                BufferOps::CreateBuffer(*gpu, initialSize, transfers | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, memoryProperties, tempBuffer, tempMemory); 
                Monolith m = { .buffer = tempBuffer, .memory = tempMemory, .memoryAllocated = 0, .capacity = initialSize};
                monoliths[VK_BUFFER_USAGE_INDEX_BUFFER_BIT] = m;
                tempBuffer = VK_NULL_HANDLE;
                tempMemory = VK_NULL_HANDLE;
            }
            if(usage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT){
                BufferOps::CreateBuffer(*gpu, initialSize, transfers | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, memoryProperties, tempBuffer, tempMemory);

                Monolith m = { .buffer = tempBuffer, .memory = tempMemory, .memoryAllocated = 0, .capacity = initialSize};
                monoliths[VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT] = m;
                tempBuffer = VK_NULL_HANDLE;
                tempMemory = VK_NULL_HANDLE;
            }
            if(usage & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT){
                BufferOps::CreateBuffer(*gpu, initialSize, transfers | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT, memoryProperties, tempBuffer, tempMemory);   
                Monolith m = { .buffer = tempBuffer, .memory = tempMemory, .memoryAllocated = 0, .capacity = initialSize};
                monoliths[VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT] = m;
                tempBuffer = VK_NULL_HANDLE;
                tempMemory = VK_NULL_HANDLE;
            }
            if(usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT){
                BufferOps::CreateBuffer(*gpu, initialSize, transfers | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, memoryProperties, tempBuffer, tempMemory);                     
                Monolith m = { .buffer = tempBuffer, .memory = tempMemory, .memoryAllocated = 0, .capacity = initialSize};
                monoliths[VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT] = m;
                tempBuffer = VK_NULL_HANDLE;
                tempMemory = VK_NULL_HANDLE;
            }
            if(usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT){
                BufferOps::CreateBuffer(*gpu, initialSize, transfers | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, memoryProperties, tempBuffer, tempMemory);          
                Monolith m = { .buffer = tempBuffer, .memory = tempMemory, .memoryAllocated = 0, .capacity = initialSize};
                monoliths[VK_BUFFER_USAGE_STORAGE_BUFFER_BIT] = m;
                tempBuffer = VK_NULL_HANDLE;
                tempMemory = VK_NULL_HANDLE;
            }
            if(usage & VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT){
                BufferOps::CreateBuffer(*gpu, initialSize, transfers | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT, memoryProperties, tempBuffer, tempMemory);    
                Monolith m = { .buffer = tempBuffer, .memory = tempMemory, .memoryAllocated = 0, .capacity = initialSize};
                monoliths[VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT] = m;
                tempBuffer = VK_NULL_HANDLE;
                tempMemory = VK_NULL_HANDLE;
            }                
        }

        virtual void AssignMonolithBuffer(BufferResource* resource, GPU* gpu, CommandManager* cm, VkMemoryPropertyFlags memoryProperties, VkDeviceSize alignment){
            
            VkBufferUsageFlags usage = resource->GetUsage();  //checked earlier when buffer resource created, just checking ag

            bool monolithExists = monoliths.contains(usage);
            if(!monolithExists){throw std::runtime_error("Fatal Error: Monolith for buffer usage not allocated");}

            auto& monolith = monoliths[usage];
            resource->SetBuffer(monolith.buffer);
            resource->SetOffSet(monolith.memoryAllocated);

            if(memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT){
                BufferOps::EnsureHostBuffer(*gpu, *cm, resource->GetData(), resource->GetDataSize(), usage, monolith.buffer, monolith.memory, monolith.memoryAllocated);
            }
            else if (memoryProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT){
                BufferOps::EnsureDeviceBuffer(*gpu, *cm, resource->GetData(), resource->GetDataSize(), usage, monolith.buffer, monolith.memory, monolith.memoryAllocated, 0);
            }

            monolith.memoryAllocated += BufferOps::AlignUp(resource->GetDataSize(), alignment);
        }

        virtual void UpdateBufferData(BufferResource* resource, GPU* gpu, CommandManager* cm){
            auto usage = resource->GetUsage();
            auto offset = resource->GetOffSet();
            auto& monolith = monoliths.at(usage);

            if(monolith.memory == VK_NULL_HANDLE){
                throw std::runtime_error("Monolith memory is null");
            }
            if(monolith.buffer == VK_NULL_HANDLE){
                throw std::runtime_error("monolith buffer is null");
            }
            if(resource->GetDataSize() > resource->GetPreviousDataSize()){
                throw std::runtime_error("Monoliths do not support memory allocation sizes changing in flight");
            }
            
            BufferOps::UpdateHostBuffer(*gpu, *cm, resource->GetData(), resource->GetDataSize(), monolith.buffer, monolith.memory, offset);

        }


        //Will try to allocate, if it's too much data, we will try to resize the buffer
        virtual void AssignMonolithBuffer(Mesh * m, GPU* gpu, CommandManager* cm, VkDeviceSize alignment){

            bool vertexMonolithExists = monoliths.contains(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            bool indiceMonolithExists = monoliths.contains(VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
            
            if(!(vertexMonolithExists &&indiceMonolithExists )){ throw std::runtime_error("vertex and/or indice monolith aren't initalized");}

            auto& vertexMonolith = monoliths.at(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            m->vertexResource->SetBuffer(vertexMonolith.buffer);
            m->vertexResource->SetMemory(vertexMonolith.memory);    
            m->vertexResource->SetOffSet(vertexMonolith.memoryAllocated);
            
            auto& indexMonolith = monoliths.at(VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
            m->indiceResource->SetBuffer(indexMonolith.buffer);
            m->indiceResource->SetMemory(indexMonolith.memory);
            m->indiceResource->SetOffSet(indexMonolith.memoryAllocated);
            
            BufferOps::EnsureDeviceBuffer(*gpu, *cm, m->vertexResource->GetData(), m->vertexResource->GetDataSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexMonolith.buffer, vertexMonolith.memory,vertexMonolith.memoryAllocated, 0);
            BufferOps::EnsureDeviceBuffer(*gpu, *cm, m->indiceResource->GetData(), m->indiceResource->GetDataSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexMonolith.buffer, indexMonolith.memory, indexMonolith.memoryAllocated, 0);

            //when incrementing the data up, we must make sure it's aligned to 256 as BufferOps does this automatically
            indexMonolith.memoryAllocated += m->indiceResource->GetAlignedDataSize(256);
            vertexMonolith.memoryAllocated += m->vertexResource->GetAlignedDataSize(256);
            
        }

     
        void Cleanup(GPU * gpu){

            //clean up the monolith buffers
            for(auto& [key,value] : monoliths){
                //free monolith memory
                if(value.buffer != VK_NULL_HANDLE){
                    vkDestroyBuffer(gpu->GetVkDevice(), value.buffer, nullptr);
                    value.buffer = VK_NULL_HANDLE;
                }
                if(value.memory != VK_NULL_HANDLE){
                    vkFreeMemory(gpu->GetVkDevice(),value.memory, nullptr);
                    value.memory = VK_NULL_HANDLE;
                }
            }

            //clean up the resources
            //delete resources allocated with new
            for(auto& [key,resource] : resourceMap){
                resource->Cleanup(gpu);
                delete(resource);  //all buffer resources allocated with new
            }
            for(auto& [key,mesh] : meshes){
                //for meshes we allocated using new for buffer resources for indices and vertexes as well as the encapsulating mesh
                mesh->indiceResource->Cleanup(gpu);
                mesh->vertexResource->Cleanup(gpu);
                delete(mesh->indiceResource);
                delete(mesh->vertexResource);
                delete(mesh); // all meshes allocated with new
            }
                 
        }

    protected:
        struct Monolith{
            VkBuffer buffer = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
            VkDeviceSize memoryAllocated;
            VkDeviceSize capacity;
        };

        std::unordered_map<VkBufferUsageFlags, Monolith> monoliths = {};
        std::unordered_map<std::string, Mesh*> meshes = {};
        std::vector<std::vector<BufferResource*>> descriptorSets = {};
        std::unordered_map<std::string, BufferResource*> resourceMap = {};

        VkBuffer tempBuffer = VK_NULL_HANDLE;     //for copying the old buffer
        VkDeviceMemory tempMemory = VK_NULL_HANDLE;
};