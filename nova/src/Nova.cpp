
#include "Nova.hpp"
#include "Shell.hpp"
#include "Renderer.hpp"
#include "VulkanEngine.hpp"
#include "GPU.hpp"
#include "PipelineLibrary.hpp"
#include "RenderpassLibrary.hpp"
#include "FramebufferLibrary.hpp"
#include "FramebufferGenerator.hpp"
#include "RenderPassManager.hpp"
#include "SwapchainManager.hpp"
#include "CommandManager.hpp"
#include "Shader.hpp"
#include "SyncManager.hpp"
#include "RenderpassLoader.hpp"
#include "PipelineLoader.hpp"
#include "DescriptorAllocator.hpp"
#include "PipelineManager.hpp"
#include "ResourceManager.hpp"
#include "DescriptorsetLoader.hpp"
#include "ImageOps.hpp"
#include <unordered_set>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace fs = std::filesystem; // safe alias at global scope for this header

Nova::Nova(
    std::shared_ptr<VulkanEngine> engine,
    std::shared_ptr<Shell> shell,
    std::shared_ptr<GPU> gpu,
    std::shared_ptr<FramebufferLibrary> framebuffersLibrary,
    std::shared_ptr<SyncManager> syncManager,
    std::shared_ptr<SwapchainManager>swapchainManager,
    std::shared_ptr<PipelineLibrary>pipelineLibrary,
    std::shared_ptr<RenderpassLibrary> renderpassLibrary,
    std::shared_ptr<CommandManager> commandManager,
    std::shared_ptr<DescriptorAllocator> descriptorAllocator,
    std::shared_ptr<ResourceManager> resourceManager,
    std::unordered_map<std::string, DescriptorFile> descriptorFiles)
    :engine(engine),
    shell(shell),
    gpu(gpu), 
    framebuffersLibrary(framebuffersLibrary),
    syncManager(syncManager),
    swapchainManager(swapchainManager),
    pipelineLibrary(pipelineLibrary),
    renderpassLibrary(renderpassLibrary),
    commandManager(commandManager),
    descriptorAllocator(descriptorAllocator),
    resourceManager(resourceManager),
    descriptorFiles(descriptorFiles)
{
    MAX_FRAMES = swapchainManager->GetImageCount();
}
Nova::~Nova(){

    this->commandManager->Cleanup();
    this->framebuffersLibrary->Cleanup();
    this->pipelineLibrary->Cleanup();
    this->descriptorAllocator->Cleanup();
    this->syncManager->Cleanup();
    this->renderpassLibrary->Cleanup();
    this->swapchainManager->Cleanup(this->gpu->GetVkDevice());
    this->resourceManager->Cleanup(gpu.get());
    this->gpu->Cleanup();
    this->engine->Cleanup();

}

void Nova::Start(){
    shell->Run(this);
}

void Nova::ObserveMouseButton(std::function<void(MouseButtonEvent)> observer) { 
    this->shell->MouseButtons()->Subscribe(observer); 
}
void Nova::ObserveMouseLocation(std::function<void(MouseMoveEvent)> observer) { 
    this->shell->MouseLocation()->Subscribe(observer);
}
void Nova::ObserveKeyPress(std::function<void(KeyPressEvent)> observer) {
    this->shell->Keys()->Subscribe(observer);
}
std::unique_ptr<IRenderLoopClient>  Nova::Builder::Build(){

    auto app = std::make_unique<Nova>(engine,shell,gpu,framebufferLibrary,syncManager,swapchainManager,pipelineLibrary,renderpassLibrary,commandManager, descriptorAllocator, resourceManager,descriptorFiles);

    auto extent = app->swapchainManager->GetExtent();
    float aspectRatio = (float)extent.width / (float)extent.height;
    glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 5.0f);                  // Camera at z = 5
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.5f);            // Looking at center of square
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);   
    app->cameraTarget = cameraTarget;
    app->cameraUp = cameraUp;
    app->camPos = camPos; 
    app->sceneCamera = {};
    app->sceneCamera.proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    app->sceneCamera.view = glm::lookAt(camPos, cameraTarget, cameraUp);
    // GLM's perspective projection produces clip space with -Z forward; Vulkan wants +Z forward.
    app->sceneCamera.proj[1][1] *= -1;

    // Camera position
    app->sceneCamera.cameraPosition = camPos;
    app->sceneCamera.padding = 0.0f;

    return app;
    
}


void Nova::InitResources() {
    
    //create 3 copies of this uniform data
    uint32_t copies = MAX_FRAMES;  
    auto cameraResource = std::make_shared<BufferResource>(
        /*usage =*/ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        /*copies = */ MAX_FRAMES,
        /*memory properties*/VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );

    cameraResource->Upload(&this->sceneCamera, sizeof(this->sceneCamera), 0);
    resourceManager->SetResource("camera", cameraResource);


    auto verticesResource = std::make_shared<BufferResource>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, MAX_FRAMES, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    auto indicesResource =  std::make_shared<BufferResource>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 1, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    auto vertices = Mesh::GenerateCubeVertices(false);
    auto indices = Mesh::GetCubeIndices();

    verticesResource->Upload(vertices.data(), vertices.size() * sizeof(VertexPC), vertices.size());
    indicesResource->Upload(indices.data(), sizeof(uint32_t) * indices.size(), indices.size());

    auto mesh = Mesh::Create(verticesResource, indicesResource);
    resourceManager->SetMesh("square", std::move(mesh));

    //create a texture resource
}

Nova::Builder& Nova::Builder::WithTextures(std::unordered_set<std::string> files) {
    
    auto texturepath = std::filesystem::path(TEXTURE_FILES_DIRECTORY);
    
    for (auto f : std::filesystem::directory_iterator(texturepath)) {
        auto filename = f.path().filename().string();
        if (files.find(filename) != files.end()) {

            int width, height, channels; unsigned char* image_data = nullptr;
            image_data = stbi_load(f.path().c_str(), &width, &height, &channels, STBI_rgb_alpha);
            assert(image_data && "image data is null");

            VkImage textureImage = VK_NULL_HANDLE;
            VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
            VkImageView textureImageView = VK_NULL_HANDLE;
            VkSampler sampler = VK_NULL_HANDLE;

            ImageOps::CreateImage(this->gpu, width,height,
                                    1,                               //mip levels
                                    1,                               //array layers                                                                      
                                    VK_SAMPLE_COUNT_1_BIT,           // samples (no MSAA)
                                    VK_FORMAT_R8G8B8A8_SRGB,         // format (common 4-channel sRGB)
                                    VK_IMAGE_TILING_OPTIMAL,         // tiling (optimal for GPU sampling)
                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, // usage (will copy data & sample)
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,  // properties (GPU local memory)
                                    VK_IMAGE_TYPE_2D,                // imageType (2D texture)
                                    VK_IMAGE_LAYOUT_UNDEFINED,       // initialLayout (undefined before transition)
                                    0,                              // createFlags (no special flags)
                                    textureImage,
                                    textureImageMemory);
            
            ImageOps::CreateImageView(this->gpu,textureImage,  
                                    VK_FORMAT_R8G8B8A8_SRGB,
                                    VK_IMAGE_ASPECT_COLOR_BIT,
                                    VK_IMAGE_VIEW_TYPE_2D,
                                    1,        // mipLevels
                                    1,        // arrayLayers
                                    textureImageView
            );

            ImageOps::CreateSampler(this->gpu,
                                    1,
                                    VK_FILTER_LINEAR,
                                    VK_FILTER_LINEAR,
                                    VK_SAMPLER_MIPMAP_MODE_LINEAR,
                                    VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                    VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                    VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                    0.0f,
                                    VK_TRUE,
                                    16.0f,
                                    0.0f,
                                    1.0f,
                                    VK_BORDER_COLOR_INT_OPAQUE_BLACK,
                                    VK_FALSE,
                                    sampler
            );


        }
      
    }
    return *this;
}

static void LoadTexture() {

}
void Nova::Init() {

    commandManager->CreateCommandPool();

    renderer = std::make_shared<Renderer>(
        shell,
        gpu,
        engine,
        swapchainManager,
        syncManager,
        pipelineLibrary,
        renderpassLibrary,
        commandManager,
        framebuffersLibrary,
        descriptorAllocator,
        resourceManager
    );

    this->resourceManager->SetDescriptorFiles(descriptorFiles);

    InitResources();
    CreateMoniliths();
    AllocateMeshes(); //allocate buffer resources that are meshes
    AllocateDescriptorResources(); //allocate buffer resources for our descriptor sets

    //get all bindings for all 
   
    //CREATING max frames size for the uniform buffer pool because we need one for each frame
    std::vector<VkDescriptorPoolSize> poolSizes = {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = MAX_FRAMES 
        }
    };
    uint32_t maxSets = MAX_FRAMES; //currently only 4 descriptors should be coming out

    descriptorAllocator->CreateDescriptorSetPool(poolSizes, maxSets, 0);
    pipelineLibrary->CreateDescriptorSetLayouts(descriptorAllocator, &descriptorFiles);

    //initializes the the resources to configure resources for 
    resourceManager->InitializeDescriptorSetsResources(pipelineLibrary); 

    //allocate descriptor sets for the following pipeline.
    renderer->AllocateAndUpdateDescriptorSets("msaa4.json");
    
    pipelineLibrary->CreatePipelines();

    commandManager->AllocateCommandBuffers();
    syncManager->Initialize(MAX_FRAMES);


    this->renderer->BindPipeline("msaa4.json");

    //set up the functor that watctches for delta x changes for camera updates
    //this is an orbital x camera - meaning i moves in a circle lookin at a spot. In this case it's the origin
    std::function<void(MouseMoveEvent)> observeX = [this](const MouseMoveEvent& delta) {

        auto frame = renderer->GetFrameIndex();
        constexpr float pitch = glm::radians(20.0f); // fixed slight tilt
        angle += cameraspeed * delta.x;
        angle = glm::mod(angle, glm::two_pi<float>());

        // Calculate camera position in spherical coordinates
        float x = orbitalDistance * cos(pitch) * cos(angle);
        float y = orbitalDistance * sin(pitch);
        float z = orbitalDistance * cos(pitch) * sin(angle);
        this->camPos = glm::vec3(x, y, z);

        // Always look at the target with a fixed up direction
        this->sceneCamera.view = glm::lookAt(camPos, cameraTarget, glm::vec3(0, 1, 0));
        auto cameraResource = this->resourceManager->GetBufferResource("camera");
        cameraResource->Upload(&this->sceneCamera, sizeof(this->sceneCamera));
        this->resourceManager->UpdateBufferData(cameraResource, gpu.get(), commandManager.get(), frame);
    };

    this->shell->MouseLocation()->MapDelta()->Subscribe(observeX);

}

void Nova::CreateMoniliths(){
    //monoliths will be created with transfers for the vertex buffer usage and index buffer usage. This is so they can be staged
    //ubo like camera don't need it

    VkBufferUsageFlags transfer = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    resourceManager->CreateMonolith(gpu.get(), commandManager.get(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, static_cast<VkDeviceSize>(256 * 1000));  //uniform monolith
    resourceManager->CreateMonolith(gpu.get(), commandManager.get(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | transfer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, static_cast<VkDeviceSize>(256 * 1000)); //index and vertice buffers with transfer operations
}

void Nova::AllocateMeshes(){
     //assign geometries
     for(auto& [key,mesh] : resourceManager->GetMeshes()){
        resourceManager->AssignMonolithBuffer(mesh.vertexResource, gpu.get(), commandManager.get(),mesh.vertexResource->MemoryProperties, static_cast<VkDeviceSize>(256));
        resourceManager->AssignMonolithBuffer(mesh.indiceResource, gpu.get(), commandManager.get(),mesh.indiceResource->MemoryProperties, static_cast<VkDeviceSize>(256));
    }
}

void Nova::AllocateDescriptorResources(){

    //assign descriptor Resources
    for(auto& [key, resource] : resourceManager->GetBufferDescriptors()){   
        resourceManager->AssignMonolithBuffer(resource, gpu.get(), commandManager.get(), resource->MemoryProperties, static_cast<VkDeviceSize>(256));
    }
}
void Nova::AllocateToMonoliths(){
   
}

void Nova::Update(float deltaTime){
   
    deltaTime = glm::clamp(deltaTime, 0.0f, 0.05f); // max ~20 FPS frame
}

void Nova::Render() {
    renderer->DrawFrame();
}
void Nova::Shutdown() {

}

Nova::Builder& Nova::Builder::WithShell(){
    shell = std::make_shared<Shell>();
    return *this;
}
Nova::Builder& Nova::Builder::WithEngine(){
    engine = std::make_shared<VulkanEngine>(shell->GetWindow(), deviceExtensions);
    return *this;

}
Nova::Builder& Nova::Builder::WithGPU(){
    gpu = std::make_shared<GPU>(engine);
    return *this;

}
Nova::Builder& Nova::Builder::WithSwapchainManager(){
    swapchainManager = SwapchainManager::Builder()
    .WithGPU(gpu)
    .WithEngine(engine)
    .WithShell(shell)
    .Build();
    swapchainManager->CreateSwapchain();
    return *this;

}

Nova::Builder& Nova::Builder::WithDescriptorAllocator(){
    descriptorAllocator = std::make_shared<DescriptorAllocator>(gpu);   
    return *this;

}

Nova::Builder& Nova::Builder::WithSyncManager(){
    syncManager = std::make_shared<SyncManager>(gpu->GetVkDevice());
    return *this; 

}
Nova::Builder& Nova::Builder::WithCommandManager(){
    commandManager = std::make_shared<CommandManager>(gpu, swapchainManager->GetImageCount());
    return *this;

}

//this gets all the locations of the files for a given directory
std::vector<std::filesystem::path>  Nova::Builder::GetAllFiles(std::string repository) {
    std::vector<std::filesystem::path> files;
    const auto directory = fs::path(repository); 
    std::cout << "Looking in: " << fs::absolute(directory) << "\n";
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            // Get the filename only
            files.push_back(entry.path());
        }
    }
    return files;
}


Nova::Builder& Nova::Builder::WithPipelineLibrary(){
    pipelineLibrary = std::make_shared<PipelineLibrary>();
    auto files = Nova::Builder::GetAllFiles(PIPELINE_FILES_DIRECTORY);
    for(auto& f : files){
        auto pipeline = PipelineManager::Create(gpu, renderpassLibrary, f.string());   
        pipelineLibrary->AddPipeline(f.filename().string(),pipeline);
    }
    return *this;
}

Nova::Builder& Nova::Builder::WithRenderpassLibrary(){

    renderpassLibrary = std::make_shared<RenderpassLibrary>();
    framebufferLibrary = std::make_shared<FramebufferLibrary>();
    auto files = Nova::Builder::GetAllFiles(RENDER_PASS_FILES_DIRECTORY);
    for(auto& f : files){
        
        auto renderpassManager = RenderPassManager::Create(gpu,f.string());
        auto framebufferGenerator = FramebufferGenerator::Create(gpu, renderpassManager, swapchainManager);
        renderpassLibrary->AddRenderpassManager(f.filename().string(), renderpassManager);    
        framebufferLibrary->AddFramebufferGenerator(f.filename().string(), framebufferGenerator);
    }
    return *this;
    
}
Nova::Builder& Nova::Builder::WithResourceManager(){
    
    this->resourceManager = std::make_shared<ResourceManager>(gpu);
    return *this;
}


Nova::Builder& Nova::Builder::WithMeshes(std::unordered_map<std::string, Mesh>&& meshes){
    
    resourceManager->SetMeshes(std::move(meshes));
    return *this;
}
Nova::Builder& Nova::Builder::WithMeshes(){
    
  
    return *this;
}

Nova::Builder& Nova::Builder::WithDescriptorSets(std::vector<std::vector<BufferResource>>&& descriptorSets){
    
    return *this;
}

Nova::Builder& Nova::Builder::WithDescriptorSets(){

    auto files = Nova::Builder::GetAllFiles(DESCRIPTOR_FILES_DIRECTORY);

    for (auto& f : files) {
        auto dfile = DescriptorsetLoader::LoadFromFile(f.string());
        auto filename = f.filename().string();
        dfile.fileName = filename;
        this->descriptorFiles[filename] = dfile;
    }
    return *this;
}
