#include "Nova.hpp"
#include "Shell.hpp"
#include "Renderer.hpp"
#include "VulkanEngine.hpp"
#include "GPU.hpp"
#include "PipelineLibrary.hpp"
#include "RenderPassManager.hpp"
#include "SwapchainManager.hpp"
#include "UBOs.hpp"
#include "CommandManager.hpp"
#include "FramebufferGenerator.hpp"
#include "Shader.hpp"
#include "SyncManager.hpp"
#include "RenderpassLoader.hpp"
#include "GraphicsPipelineLoader.hpp"
#include "DescriptorAllocator.hpp"
#include "PipelineManager.hpp"
#include "ResourceManager.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static std::vector<VertexPC> GenerateCubeVertices(bool solidColor = false) {
    auto randColor = []() -> glm::vec4 {
        return glm::vec4(
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            1.0f
        );
    };

    glm::vec4 colorForAll = randColor(); // used if solidColor == true

    auto getColor = [&]() -> glm::vec4 {
        return solidColor ? colorForAll : randColor();
    };

    return {
        // Front face
        {{-0.5f, -0.5f,  0.5f}, getColor()}, // 0
        {{ 0.5f, -0.5f,  0.5f}, getColor()}, // 1
        {{ 0.5f,  0.5f,  0.5f}, getColor()}, // 2
        {{-0.5f,  0.5f,  0.5f}, getColor()}, // 3

        // Back face
        {{-0.5f, -0.5f, -0.5f}, getColor()}, // 4
        {{ 0.5f, -0.5f, -0.5f}, getColor()}, // 5
        {{ 0.5f,  0.5f, -0.5f}, getColor()}, // 6
        {{-0.5f,  0.5f, -0.5f}, getColor()}, // 7
    };
}

static std::vector<uint16_t> GetCubeIndices() {
    return {
        // Front face
        0, 1, 2, 2, 3, 0,

        // Right face
        1, 5, 6, 6, 2, 1,

        // Back face
        5, 4, 7, 7, 6, 5,

        // Left face
        4, 0, 3, 3, 7, 4,

        // Top face
        3, 2, 6, 6, 7, 3,

        // Bottom face
        4, 5, 1, 1, 0, 4
    };
}
Nova::Nova(
    std::shared_ptr<VulkanEngine> engine,
    std::shared_ptr<Shell> shell,
    std::shared_ptr<GPU> gpu,
    std::shared_ptr<FramebufferGenerator> framebuffersContainer,
    std::shared_ptr<SyncManager> syncManager,
    std::shared_ptr<SwapchainManager>swapchainManager,
    std::shared_ptr<PipelineManager> pipelineManager,
    std::shared_ptr<PipelineLibrary>pipelineLibrary,
    std::shared_ptr<RenderPassManager> renderpassManager,
    std::shared_ptr<CommandManager> commandManager,
    std::shared_ptr<DescriptorAllocator> descriptorAllocator,
    std::shared_ptr<ResourceManager> resourceManager)
    :engine(engine),
    shell(shell),
    gpu(gpu), 
    framebuffersContainer(framebuffersContainer),
    syncManager(syncManager),
    swapchainManager(swapchainManager),
    pipelineManager(pipelineManager),
    pipelineLibrary(pipelineLibrary),
    renderpassManager(renderpassManager),
    commandManager(commandManager),
    descriptorAllocator(descriptorAllocator),
    resourceManager(resourceManager)
{
    
}
Nova::~Nova(){

    this->commandManager->Cleanup();
    this->framebuffersContainer->Cleanup(this->gpu->GetVkDevice());
    this->pipelineManager->Cleanup();
    this->descriptorAllocator->Cleanup();
    this->syncManager->Cleanup();
    this->renderpassManager->Cleanup();
    this->swapchainManager->Cleanup(this->gpu->GetVkDevice());
    this->resourceManager->Cleanup();
    this->engine->Cleanup();
    this->resourceManager->Cleanup();

    //delete resources allocated with new
    for(auto& [key,value] : resourceManager->GetResourceMap()){
        delete(value);
    }
    for(auto& [key,value] : resourceManager->GetMeshes()){
        //for meshes we allocated using new for buffer resources for indices and vertexes as well as the encapsulating mesh
        delete(value->indiceResource);
        delete(value->vertexResource);
        delete(value);
    }
}

void Nova::Start(){
    shell->Run(this);
}

std::unique_ptr<IRenderLoopClient>  Nova::Builder::Build(){

    auto app = std::make_unique<Nova>(engine,shell,gpu,framebuffersContainer,syncManager,swapchainManager,pipelineManager,pipelineLibrary,renderpassManager,commandManager, descriptorAllocator, resourceManager);
    app->camera = this->camera;
    app->cameraTarget = this->cameraTarget;
    app->cameraUp = this->cameraUp;
    app->camPos = this->camPos;
    return app;
    
}



void Nova::Init() {
    
    renderer = std::make_shared<Renderer>(
        shell,
        gpu,
        engine,
        swapchainManager,
        syncManager,
        pipelineManager,
        renderpassManager,
        commandManager,
        framebuffersContainer,
        descriptorAllocator,
        resourceManager
    );

    pipelineManager->WithDescriptorSetPool();
    pipelineManager->WithDescriptorSetLayout();

    pipelineManager->CreateGraphicsPipeline<VertexPC>();
    commandManager->CreateCommandPool();
    commandManager->AllocateCommandBuffers();
    syncManager->Initialize(MAX_FRAMES);
    CreateMoniliths();
    AllocateToMonoliths();
    descriptorAllocator->AllocateDescriptorSets();
    descriptorAllocator->UpdateDescriptorSets(resourceManager.get());  

}

void Nova::CreateMoniliths(){
    //monoliths will be created with transfers for the vertex buffer usage and index buffer usage. This is so they can be staged
    //ubo like camera don't need it
    VkBufferUsageFlags transfer = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    resourceManager->CreateMonolith(gpu.get(), commandManager.get(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 256 * 1000); 
    resourceManager->CreateMonolith(gpu.get(), commandManager.get(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | transfer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 256 * 1000); 
}

void Nova::AllocateToMonoliths(){
    //assign resources like camera or other resources that uses buffers and memory
    auto cameraResource = resourceManager->GetResource("camera");
    resourceManager->AssignMonolithBuffer(cameraResource, gpu.get(), commandManager.get(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, static_cast<VkDeviceSize>(256));

    //assign geometries
     for(auto& [key,value] : resourceManager->GetMeshes()){
        resourceManager->AssignMonolithBuffer(value, gpu.get(), commandManager.get(), 256);
    }
}

void Nova::Update(float deltaTime){

    //camera orbiting and looking at the origin
    angle += cameraspeed * deltaTime;
    angle = glm::mod(angle, glm::two_pi<float>());

    camPos.x = glm::cos(angle) * oribitalDistance;
    camPos.z = glm::sin(angle) * oribitalDistance;

    camera.view = glm::lookAt(camPos, cameraTarget, cameraUp);
    auto cameraResource = this->resourceManager->GetResource("camera");
    cameraResource->Upload(&camera, sizeof(camera));
    this->resourceManager->UpdateBufferData(cameraResource, gpu.get(),commandManager.get());
    
    
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
    engine = std::make_shared<VulkanEngine>(shell->GetWindow());
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

    return *this;

}
Nova::Builder& Nova::Builder::WithRenderpass(){
    renderpassManager = std::make_shared<RenderPassManager>(gpu);
    return *this;

}
Nova::Builder& Nova::Builder::WithFramebufferGenerator(){
    framebuffersContainer = std::make_shared<FramebufferGenerator>(
                                                    gpu->GetVkDevice(), 
                                                    renderpassManager->GetRenderPass(),
                                                    swapchainManager);
    return *this;

}
Nova::Builder& Nova::Builder::WithDescriptorAllocator(){
    descriptorAllocator = std::make_shared<DescriptorAllocator>(gpu);   
    return *this;

}
Nova::Builder& Nova::Builder::WithPipelineManager(){
    pipelineManager = std::make_shared<PipelineManager>(gpu,renderpassManager,descriptorAllocator);  
    return *this;
}
Nova::Builder& Nova::Builder::WithSyncManager(){
    syncManager = std::make_shared<SyncManager>(gpu->GetVkDevice());
    return *this; 

}
Nova::Builder& Nova::Builder::WithCommandManager(){
    commandManager = std::make_shared<CommandManager>(gpu);
    return *this;

}

Nova::Builder& Nova::Builder::WithPipelineLibrary(){
    pipelineLibrary = std::make_shared<PipelineLibrary>();
    return *this;
}

Nova::Builder& Nova::Builder::WithResourceManager(){
    
    this->resourceManager = std::make_shared<ResourceManager>();
    return *this;
}

Nova::Builder& Nova::Builder::WithResourceMap(std::unordered_map<std::string, BufferResource*> resourceMap){
    resourceManager->SetResourceMap(resourceMap);
    return *this;
}

Nova::Builder& Nova::Builder::WithResourceMap(){

    camera.view = glm::lookAt(camPos, cameraTarget, cameraUp);

    // Perspective projection matrix
    auto extent = swapchainManager->GetExtent();
    float aspectRatio = (float)extent.width / (float)extent.height;
    camera.proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    // GLM's perspective projection produces clip space with -Z forward; Vulkan wants +Z forward.
    camera.proj[1][1] *= -1;

    // Camera position
    camera.cameraPosition = camPos;
    camera.padding = 0.0f;

    auto cameraResource = BufferResource::Create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,0 ,0);  //descriptor set 0 with binding 0
    cameraResource->Upload(&camera, sizeof(camera), 0);
    resourceManager->SetResource("camera",cameraResource);
    return *this;
}

Nova::Builder& Nova::Builder::WithMeshes(std::unordered_map<std::string, Mesh*> meshes){
    
    resourceManager->SetMeshes(meshes);
    return *this;
}
Nova::Builder& Nova::Builder::WithMeshes(){
    
    auto verticesResource = BufferResource::Create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    auto indicesResource = BufferResource::Create(VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    auto indices = GetCubeIndices();
    auto vertices = GenerateCubeVertices(true);
    verticesResource->Upload(vertices.data(),vertices.size() * sizeof(VertexPC), vertices.size());
    indicesResource->Upload(indices.data(), sizeof(uint32_t) * indices.size(),indices.size());
    auto mesh = Mesh::Create(verticesResource, indicesResource);
    resourceManager->SetMesh("square", mesh);
    return *this;
}

Nova::Builder& Nova::Builder::WithDescriptorSets(std::vector<std::vector<BufferResource*>>& descriptorSets){
    
    resourceManager->SetDescriptorSets(descriptorSets);
    return *this;
}

Nova::Builder& Nova::Builder::WithDescriptorSets(){
    std::vector<std::vector<BufferResource*>> descriptorSets;
    std::vector<BufferResource*> set0;

    //set 0 binding 0
    auto descriptor = resourceManager->GetResource("camera");
    set0.push_back(descriptor);
    descriptorSets.push_back(set0);
    resourceManager->SetDescriptorSets(descriptorSets);
    return *this;
}
