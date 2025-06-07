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
#include "Mesh.hpp"
#include "CommandManager.hpp"
#include "Shader.hpp"
#include "SyncManager.hpp"
#include "RenderpassLoader.hpp"
#include "GraphicsPipelineLoader.hpp"
#include "DescriptorAllocator.hpp"
#include "PipelineManager.hpp"
#include "ResourceManager.hpp"

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
    std::shared_ptr<ResourceManager> resourceManager)
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
    resourceManager(resourceManager)
{
    
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

std::unique_ptr<IRenderLoopClient>  Nova::Builder::Build(){

    auto app = std::make_unique<Nova>(engine,shell,gpu,framebufferLibrary,syncManager,swapchainManager,pipelineLibrary,renderpassLibrary,commandManager, descriptorAllocator, resourceManager);

    glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 5.0f);                  // Camera at z = 5
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.5f);            // Looking at center of square
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);   
    app->cameraTarget = cameraTarget;
    app->cameraUp = cameraUp;
    app->camPos = camPos; 
    app->camera = {};

    return app;
    
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
    MAX_FRAMES = swapchainManager->GetImageCount();
    auto extent = swapchainManager->GetExtent();
    float aspectRatio = (float)extent.width / (float)extent.height;

    CameraUBO c = {};
    c.proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    
    c.view = glm::lookAt(camPos, cameraTarget, cameraUp);
    // GLM's perspective projection produces clip space with -Z forward; Vulkan wants +Z forward.
    c.proj[1][1] *= -1;

    // Camera position
    c.cameraPosition = camPos;
    c.padding = 0.0f;

    auto cameraResource = BufferResource::Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,MAX_FRAMES, 0 ,0);  // max frames and descriptor set 0 with binding 0
    cameraResource->Upload(&c, sizeof(c), 0);
    resourceManager->SetResource("camera",cameraResource);

    camera = std::vector<CameraUBO>(MAX_FRAMES, c);
   

    auto verticesResource = BufferResource::Create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, MAX_FRAMES);
    auto indicesResource = BufferResource::Create(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 1);
    auto indices = Mesh::GetCubeIndices();
    auto vertices = Mesh::GenerateCubeVertices(true);
    verticesResource->Upload(vertices.data(),vertices.size() * sizeof(VertexPC), vertices.size());
    indicesResource->Upload(indices.data(), sizeof(uint32_t) * indices.size(),indices.size());
    auto mesh = Mesh::Create(verticesResource, indicesResource);
    resourceManager->SetMesh("square", mesh);

    CreateMoniliths();
    AllocateMeshes(); //allocate buffer resources that are meshes
    AllocateDescriptorSets(); //allocate buffer resources that are descriptor sets

    //get all bindings for each pipeline to create our pool
    std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorBindingsPerSet;
    for(auto& [key, pipelineManager] : pipelineLibrary->GetPipelines()){
        auto bindings = pipelineManager->GetDescriptorSetBindings();
        descriptorBindingsPerSet.insert(descriptorBindingsPerSet.begin(), bindings.begin(), bindings.end());
    }

    descriptorAllocator->CreateDescriptorSetPool(descriptorBindingsPerSet, MAX_FRAMES);
    pipelineLibrary->CreateDescriptorSetLayouts(descriptorAllocator);

    //take descriptor names specified with the descriptor sets in the pipeline config and create buffer resources that map to descriptor sets for all pipelines
    resourceManager->InitializeDescriptorSetsResources(pipelineLibrary); 

    renderer->AllocateDescriptorSets("msaa4.json");
    
    
    pipelineLibrary->CreatePipelines();

    commandManager->AllocateCommandBuffers();
    syncManager->Initialize(MAX_FRAMES);


    this->renderer->BindPipeline("msaa4.json");

}

void Nova::CreateMoniliths(){
    //monoliths will be created with transfers for the vertex buffer usage and index buffer usage. This is so they can be staged
    //ubo like camera don't need it
    VkBufferUsageFlags transfer = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    resourceManager->CreateMonolith(gpu.get(), commandManager.get(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 256 * 1000); 
    resourceManager->CreateMonolith(gpu.get(), commandManager.get(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | transfer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 256 * 1000); 
}

void Nova::AllocateMeshes(){
     //assign geometries
     for(auto& [key,mesh] : resourceManager->GetMeshes()){
        resourceManager->AssignMonolithBuffer(mesh->vertexResource, gpu.get(), commandManager.get(),VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, static_cast<VkDeviceSize>(256));
        resourceManager->AssignMonolithBuffer(mesh->indiceResource, gpu.get(), commandManager.get(),VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, static_cast<VkDeviceSize>(256));
    }
}

void Nova::AllocateDescriptorSets(){
     //assign resources like camera or other resources that uses buffers and memory
    auto cameraResource = resourceManager->GetResource("camera");
    resourceManager->AssignMonolithBuffer(cameraResource, gpu.get(), commandManager.get(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, static_cast<VkDeviceSize>(256));
}
void Nova::AllocateToMonoliths(){
   
}

void Nova::Update(float deltaTime){

    auto frame = renderer->GetFrameIndex();
    deltaTime = glm::clamp(deltaTime, 0.0f, 0.05f); // max ~20 FPS frame

    //camera orbiting and looking at the origin
    float pitch = glm::radians(20.0f); // fixed slight tilt
    angle += cameraspeed * deltaTime;
    angle = glm::mod(angle, glm::two_pi<float>());

    // Calculate camera position in spherical coordinates
    float x = orbitalDistance * cos(pitch) * cos(angle);
    float y = orbitalDistance * sin(pitch);
    float z = orbitalDistance * cos(pitch) * sin(angle);
    camPos = glm::vec3(x, y, z);

    // Always look at the target with a fixed up direction
    camera[frame].view = glm::lookAt(camPos, cameraTarget, glm::vec3(0, 1, 0));
    auto cameraResource = this->resourceManager->GetResource("camera");
    cameraResource->Upload(&camera[frame], sizeof(camera[frame]));
    this->resourceManager->UpdateBufferData(cameraResource, gpu.get(),commandManager.get(), frame);

    //std::cout << "deltaTime: " << deltaTime << ", angle: " << glm::degrees(angle) << std::endl;
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

//this gets all the locations of the fil0es for a given directory
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

Nova::Builder& Nova::Builder::WithResourceMap(std::unordered_map<std::string, BufferResource*> resourceMap){
    resourceManager->SetResourceMap(resourceMap);
    return *this;
}

Nova::Builder& Nova::Builder::WithResourceMap(){

    // camera.view = glm::lookAt(camPos, cameraTarget, cameraUp);

    // // Perspective projection matrix
    // auto extent = swapchainManager->GetExtent();
    // float aspectRatio = (float)extent.width / (float)extent.height;
    // camera.proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    // // GLM's perspective projection produces clip space with -Z forward; Vulkan wants +Z forward.
    // camera.proj[1][1] *= -1;

    // // Camera position
    // camera.cameraPosition = camPos;
    // camera.padding = 0.0f;

    // auto cameraResource = BufferResource::Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 4, 0 ,0);  // 4 frames and descriptor set 0 with binding 0
    // cameraResource->Upload(&camera, sizeof(camera), 0);
    // resourceManager->SetResource("camera",cameraResource);
    return *this;
}

Nova::Builder& Nova::Builder::WithMeshes(std::unordered_map<std::string, Mesh*> meshes){
    
    resourceManager->SetMeshes(meshes);
    return *this;
}
Nova::Builder& Nova::Builder::WithMeshes(){
    
  
    return *this;
}

Nova::Builder& Nova::Builder::WithDescriptorSets(std::vector<std::vector<BufferResource*>>& descriptorSets){
    
    //resourceManager->SetDescriptorSets(descriptorSets);
    return *this;
}

Nova::Builder& Nova::Builder::WithDescriptorSets(){

    //all descriptor sets will be set here
    //we set descriptorsets for pipelines
    
    // std::vector<std::vector<BufferResource*>> descriptorSets;
    // std::vector<BufferResource*> set0;

    // //set 0 binding 0
    // auto descriptor = resourceManager->GetResource("camera");
    // set0.push_back(descriptor);
    // descriptorSets.push_back(set0);
    // resourceManager->SetDescriptorSets("defaultPipelineConfig copy.json", descriptorSets);
    return *this;
}
