#include "ResourceManager.hpp"
#include "PipelineManager.hpp"
#include "PipelineLibrary.hpp"
#include "BufferOps.hpp"
#include "BufferResource.hpp"
#include "Mesh.hpp"
#include "GPU.hpp"

ResourceManager::ResourceManager(std::shared_ptr<GPU> gpu) :gpu(gpu){
        
}

void ResourceManager::SetMesh(std::string key, Mesh* mesh){
    meshes[key] = mesh;
}   
Mesh* ResourceManager::GetMesh(std::string key){
    return meshes[key];
}
void ResourceManager::SetMeshes(std::unordered_map<std::string, Mesh*>& meshes){
    this->meshes = meshes;
}
std::unordered_map<std::string, Mesh*>& ResourceManager::GetMeshes(){
    return meshes;
}
void ResourceManager::SetDescriptorSets(std::string pipelineKey, std::vector<std::vector<BufferResource*>> descriptorSets){
    this->pipelineDescriptorSets[pipelineKey] = descriptorSets;
}

std::vector<BufferResource*>& ResourceManager::GetDescriptorSet(std::string pipelineKey, uint32_t set){
    return pipelineDescriptorSets[pipelineKey][set];         
}    

std::vector<std::vector<BufferResource*>>& ResourceManager::GetDescriptorSets(std::string pipelineKey){
    return pipelineDescriptorSets[pipelineKey];
}

void ResourceManager::SetResourceMap(std::unordered_map<std::string, BufferResource*>& resourceMap){
    this->resourceMap = resourceMap;
}
std::unordered_map<std::string, BufferResource*>& ResourceManager::GetResourceMap(){
    return this->resourceMap;
}

BufferResource* ResourceManager::GetResource(std::string key){
    return resourceMap[key];
}

void ResourceManager::SetResource(std::string key, BufferResource* r){
    resourceMap[key] = r;
}

//this fills the descriptor map below. We aredn't creating new resources, just another way to access them
//this creates a map to access descriptor set resources by set and binding and pipeline based on the names specified in the pipeline config
//for ex - if a resource named "camera" is specified in the descriptor set, it expects that resource in the resource map
                                                
void ResourceManager::InitializeDescriptorSetsResources(std::shared_ptr<PipelineLibrary> pipelineLibrary){

    //for each pipeline manager, we will create descriptorsets for it
    auto pipelineManagers = pipelineLibrary->GetPipelines();

    for(auto& [pipelineName, manager] : pipelineManagers){
        std::vector<std::vector<BufferResource*>> descriptorSets;
        auto descriptorFileName = manager->GetDescriptorFileName();
        auto descriptorFile = descriptorFiles[descriptorFileName];
        auto descriptorNames = descriptorFile.descriptorNames;
        for (int i = 0; i < descriptorNames.size(); i++) {

            std::vector<BufferResource*> bindings;          
            for(auto name : descriptorNames[i]) {
                bindings.push_back(resourceMap.at(name));
            }
            descriptorSets.push_back(bindings);
        }
        pipelineDescriptorSets[pipelineName] = descriptorSets;
    }
}

//for frame, it is for updating descriptorSets that have per frame data
//if you this descriptor set isn't per frame, just use 1
//the overall idea behind vkupdatedescriptor sets is to tell the descriptorset where the buffer resource data is
void ResourceManager::UpdateDescriptorSet(std::string pipelineKey,
                                            VkDescriptorSet descriptorSet, 
                                            VkDescriptorSetLayout descriptorLayout,
                                            std::vector<VkDescriptorSetLayoutBinding> pipelineDescriptorBindings,
                                            uint32_t setIndex,
                                            uint32_t frameIndex){
     
    auto setResources = GetDescriptorSet(pipelineKey, setIndex);
    std::vector<VkWriteDescriptorSet> writes;
    std::vector<VkDescriptorBufferInfo> bufferInfos;
    
    if(setResources.size() != pipelineDescriptorBindings.size()){ 
        throw std::runtime_error("descriptor resources for set is unequal to the bindings for it. Make sure config bindings and actual buffer resources are equal");
    }

    for (uint32_t binding = 0; binding < setResources.size(); ++binding) {
        
        auto initialOffset = setResources[binding]->GetOffSet();
        auto offset = initialOffset + setResources[binding]->GetAlignedDataSize(256) * frameIndex;
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = setResources[binding]->GetBuffer();
        bufferInfo.offset = setResources[binding]->GetOffSet();
        bufferInfo.range  = setResources[binding]->GetAlignedDataSize(256);

        bufferInfos.push_back(bufferInfo);

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = descriptorSet;
        write.dstBinding = binding;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufferInfos.back();

        writes.push_back(write);
    }

    vkUpdateDescriptorSets(gpu->GetVkDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

//Create resources for a type of data
//if usage isn't created with desttransfer or src transfer then you will be warned, no exception
//if multiple flags are specified (vertex, index, etc) we will create a usage with each one
void ResourceManager::CreateMonolith(GPU* gpu, CommandManager* commandManager, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkDeviceSize initialSize){

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

//assigning a monolith buffer will copy the data to a specified monolith buffer
//this function will create memory and a buffer for the number of copies requested
//when we create a buffer resource it's mandated to have a copies argument for the copy count
//if your buffer resource is created with 3 frames, this will create memory for 3 of the resource
void ResourceManager::AssignMonolithBuffer(BufferResource& resource, GPU* gpu, CommandManager* cm, VkMemoryPropertyFlags memoryProperties, VkDeviceSize alignment){
    
    VkBufferUsageFlags usage = resource.GetUsage();  //checked earlier when buffer resource created, just checking again

    bool monolithExists = monoliths.contains(usage);
    if(!monolithExists){throw std::runtime_error("Fatal Error: Monolith for buffer usage not allocated");}

    auto& monolith = monoliths[usage];
    resource.SetBuffer(monolith.buffer); 

    auto copies = resource.GetCopyCount(); //the amount of copies of the resource we requesting for the buffer 
    resource.SetOffSet(monolith.memoryAllocated);

    for(int copy = 0; copy < copies; copy++){
        if(memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT){
            BufferOps::EnsureHostBuffer(*gpu, *cm, resource.GetData(), resource.GetDataSize(), usage, monolith.buffer, monolith.memory, monolith.memoryAllocated);
        }
        else if (memoryProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT){
            BufferOps::EnsureDeviceBuffer(*gpu, *cm, resource.GetData(), resource.GetDataSize(), usage, monolith.buffer, monolith.memory, monolith.memoryAllocated, 0);
        }

        monolith.memoryAllocated += BufferOps::AlignUp(resource.GetDataSize(), alignment);
    }
}

void ResourceManager::UpdateBufferData(BufferResource& resource, GPU* gpu, CommandManager* cm, uint32_t frame){

    auto usage = resource.GetUsage();
    auto alignedSize = resource.GetAlignedDataSize(256);
    auto initialOffset = resource.GetOffSet();
    auto offset = static_cast<VkDeviceSize> (initialOffset + alignedSize * frame);

    auto& monolith = monoliths.at(usage);

    if(monolith.memory == VK_NULL_HANDLE){
        throw std::runtime_error("Monolith memory is null");
    }
    if(monolith.buffer == VK_NULL_HANDLE){
        throw std::runtime_error("monolith buffer is null");
    }
    if(resource.GetDataSize() > resource.GetPreviousDataSize()){
        throw std::runtime_error("Monoliths do not support memory allocation sizes changing in flight");
    }
    
    BufferOps::UpdateHostBuffer(*gpu, *cm, resource.GetData(), resource.GetDataSize(), monolith.buffer, monolith.memory, offset);

}


void ResourceManager::Cleanup(GPU * gpu){

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