#include "ResourceManager.hpp"
#include "PipelineManager.hpp"
#include "PipelineLibrary.hpp"
#include "BufferOps.hpp"
#include "GPU.hpp"
#include <cassert>
#include "ImageOps.hpp"
ResourceManager::ResourceManager(std::shared_ptr<GPU> gpu) :gpu(gpu){
        
}

void ResourceManager::SetMesh(std::string key, Mesh&& mesh){
    meshes.insert({ key, std::move(mesh) });
}   
Mesh& ResourceManager::GetMesh(std::string key){
    return meshes.at(key);
}
void ResourceManager::SetMeshes(std::unordered_map<std::string, Mesh>&& meshes){
    this->meshes = std::move(meshes);
}
std::unordered_map<std::string, Mesh>& ResourceManager::GetMeshes(){
    return meshes;
}
void ResourceManager::SetDescriptorSets(string pipelineKey, vector<vector<shared_ptr<Descriptor>>> && descriptorSets){
    this->pipelineDescriptorSets.insert({ pipelineKey, std::move(descriptorSets) });
}

vector<shared_ptr<Descriptor>>& ResourceManager::GetDescriptorSet(string pipelineKey, uint32_t set){
    return pipelineDescriptorSets.at(pipelineKey).at(set);         
}    

vector<vector<shared_ptr<Descriptor>>>& ResourceManager::GetDescriptorSets(string pipelineKey){
    return pipelineDescriptorSets.at(pipelineKey);
}

std::shared_ptr<BufferResource> ResourceManager::GetBufferResource(std::string key){
    return bufferDescriptors.at(key);
}

void ResourceManager::SetBufferResource(string key, shared_ptr<BufferResource> r){
    bufferDescriptors.insert({ key, r });
}

shared_ptr<ImageResource> ResourceManager::GetImageResource(string key){
    return imageDescriptors.at(key);
}
void ResourceManager::SetImageResource(string key, shared_ptr<ImageResource> r){
    imageDescriptors.insert({ key, r });
}

void ResourceManager::SetResource(std::string key, std::shared_ptr<Resource> resource){
    resources.insert({ key, resource });
    if(resource->resourceType == ResourceType::BUFFER){
        bufferDescriptors.insert({key, std::static_pointer_cast<BufferResource>(resource)});
    }
    else if(resource->resourceType == ResourceType::IMAGE){
        imageDescriptors.insert({key, std::static_pointer_cast<ImageResource>(resource)});
    }
    else{
        assert(false && "unknown resource type");
    }
}
std::shared_ptr<Resource> ResourceManager::GetResource(std::string key){
    return resources.at(key);
}
//this fills the descriptor map below. We aredn't creating new resources, just another way to access them
//this creates a map to access descriptor set resources by set and binding and pipeline based on the names specified in the pipeline config
//for ex - if a resource named "camera" is specified in the descriptor set, it expects that resource in the resource map
                                                
void ResourceManager::InitializeDescriptorSetsResources(std::shared_ptr<PipelineLibrary> pipelineLibrary){

    //for each pipeline manager, we will create descriptorsets for it
    auto pipelineManagers = pipelineLibrary->GetPipelines();

    for(auto& [pipelineName, manager] : pipelineManagers){
        vector<vector<shared_ptr<Descriptor>>>  descriptorSets;
        auto descriptorFileName = manager->GetDescriptorFileName();
        auto descriptorFile = descriptorFiles[descriptorFileName];
        auto descriptorNames = descriptorFile.descriptorNames; //flattened 2d vector of descriptor names
        
        for (uint32_t set = 0; set < descriptorNames.size(); set++) {

            vector<shared_ptr<Descriptor>> bindings;          
            for(uint32_t binding = 0; binding < descriptorNames[set].size(); binding++) {
                auto descriptorCount = descriptorFile.setsBindings[set][binding].descriptorCount;
                auto descriptorType = descriptorFile.setsBindings[set][binding].descriptorType;
                auto name = descriptorNames[set][binding];
                auto& resource = this->resources.at(name);

                if(resource->resourceType == ResourceType::BUFFER){
                    std::shared_ptr<Descriptor> r = std::make_shared<BufferDescriptor>(set,binding, descriptorCount, descriptorType, bufferDescriptors.at(name));
                    bindings.push_back(r);
                }
                else if(resource->resourceType == ResourceType::IMAGE){
                    auto mipLevels = descriptorFile.requestedMipLevels[set][binding];
                    auto layers = descriptorFile.requestedLayers[set][binding];
                    assert(layers && mipLevels);

                    std::shared_ptr<Descriptor> r = std::make_shared<ImageDescriptor>(set,binding, descriptorCount, descriptorType,layers.value(), mipLevels.value(), imageDescriptors.at(name));
                    bindings.push_back(r);
                }
                else{
                    assert(false && "Invalid Resource Type");
                }  
            }
            descriptorSets.push_back(bindings);
        }
        pipelineDescriptorSets.insert({pipelineName, descriptorSets});
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
     
    auto& setResources = GetDescriptorSet(pipelineKey, setIndex);
    std::vector<VkWriteDescriptorSet> writes;
    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkDescriptorImageInfo> imageInfos;
    
    if(setResources.size() != pipelineDescriptorBindings.size()){ 
        throw std::runtime_error("descriptor resources for set is unequal to the bindings for it. Make sure config bindings and actual buffer resources are equal");
    }

    for (uint32_t binding = 0; binding < setResources.size(); ++binding) {
        auto descriptor = setResources[binding];
        auto resource = descriptor->GetResource();
        if (auto ptr = std::get_if<std::shared_ptr<BufferResource>>(&resource)) {
            auto bufferResource = *ptr;
            auto initialOffset = bufferResource->Offset;
            auto offset = initialOffset + bufferResource->GetAlignedDataSize(256) * frameIndex;
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = bufferResource->Buffer;
            bufferInfo.offset = offset;
            bufferInfo.range  = bufferResource->GetAlignedDataSize(256);
            

            bufferInfos.push_back(bufferInfo);

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = descriptorSet;
            write.dstBinding = binding;
            write.dstArrayElement = 0;
            write.descriptorType = setResources[binding]->DescriptorType;
            write.descriptorCount = setResources[binding]->DescriptorCount;
            write.pBufferInfo = &bufferInfos.back();

            writes.push_back(write);
        }
        else if (auto ptr = std::get_if<std::shared_ptr<ImageResource>>(&resource)) {
            auto imageResource = *ptr;
            auto imageDescriptor = static_pointer_cast<ImageDescriptor>(descriptor);
            auto key = imageDescriptor->key;
            auto imageBundle = imageResource->GetTextureResolution(key);
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // or imageResource->ImageLayout    
            imageInfo.imageView = imageBundle.ImageView;
            imageInfo.sampler = imageBundle.Sampler;

            imageInfos.push_back(imageInfo);

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = descriptorSet;
            write.dstBinding = binding;
            write.dstArrayElement = 0;
            write.descriptorType = setResources[binding]->DescriptorType; // likely VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            write.descriptorCount = setResources[binding]->DescriptorCount;
            write.pImageInfo = &imageInfos.back();

            writes.push_back(write);
        }
        else {
            assert(false && "only 2 types of resources ATM");
        }
    
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
void ResourceManager::AssignMonolithBuffer(std::shared_ptr<BufferResource> resource, GPU* gpu, CommandManager* cm, VkMemoryPropertyFlags memoryProperties, VkDeviceSize alignment){
    
    VkBufferUsageFlags usage = resource->Usage;  //checked earlier when buffer resource created, just checking again

    bool monolithExists = monoliths.contains(usage);
    assert(monolithExists && "monolith does not exist");

    auto& monolith = monoliths[usage];
    resource->Buffer = monolith.buffer; 

    auto copies = resource->Copies; //the amount of copies of the resource we requesting for the buffer 
    resource->Offset = monolith.memoryAllocated;

    for(int copy = 0; copy < copies; copy++){
        if(memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT){
            BufferOps::EnsureHostBuffer(*gpu, *cm, resource->Data, resource->DataSize, usage, monolith.buffer, monolith.memory, monolith.memoryAllocated);
        }
        else if (memoryProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT){
            BufferOps::EnsureDeviceBuffer(*gpu, *cm, resource->Data, resource->DataSize, usage, monolith.buffer, monolith.memory, monolith.memoryAllocated, 0);
        }

        monolith.memoryAllocated += BufferOps::AlignUp(resource->DataSize, alignment);
    }
}

void ResourceManager::UpdateBufferData(std::shared_ptr<BufferResource> resource, GPU* gpu, CommandManager* cm, uint32_t frame){

    auto usage = resource->Usage;
    auto alignedSize = resource->GetAlignedDataSize(256);
    auto initialOffset = resource->Offset;
    auto offset = static_cast<VkDeviceSize> (initialOffset + alignedSize * frame);

    auto& monolith = monoliths.at(usage);

    assert(monolith.memory != VK_NULL_HANDLE);
    assert(monolith.buffer != VK_NULL_HANDLE);
    assert(resource->DataSize == resource->PreviousSize);
    
    BufferOps::UpdateHostBuffer(*gpu, *cm, resource->Data, resource->DataSize, monolith.buffer, monolith.memory, offset);

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
    for(auto& [key,resource] : resources){
        resource->Cleanup();
        //delete(resource);  //all buffer resources allocated with new
    }
    for(auto& [key,mesh] : meshes){
        //for meshes we allocated using new for buffer resources for indices and vertexes as well as the encapsulating mesh
        mesh.indiceResource->Cleanup();
        mesh.vertexResource->Cleanup();
        //delete(mesh->indiceResource);
        //delete(mesh->vertexResource);
        //delete(mesh); // all meshes allocated with new
    }

    //clean staging buffer if it exists
    //kill staging buffer 
    if (ImageOps::stagingBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(gpu->GetVkDevice(), ImageOps::stagingBuffer, nullptr);
        vkFreeMemory(gpu->GetVkDevice(), ImageOps::stagingMemory, nullptr);
    }

}