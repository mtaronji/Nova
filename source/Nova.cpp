#include "Nova.hpp"

Nova::Nova(
    std::shared_ptr<VulkanEngine> engine,
    std::shared_ptr<Shell> shell,
    std::shared_ptr<GPU> gpu,
    std::shared_ptr<FramebufferGenerator> framebuffersContainer,
    std::shared_ptr<SyncManager> syncManager,
    std::shared_ptr<SwapchainManager>swapchainManager,
    std::shared_ptr<PipelineLibrary>pipelineLibrary,
    std::shared_ptr<RenderPassManager> renderpassManager,
    std::shared_ptr<CommandManager> commandManagers)
    :engine(engine),
    shell(shell),
    gpu(gpu), 
    framebuffersContainer(framebuffersContainer),
    syncManager(syncManager),
    swapchainManager(swapchainManager),
    pipelineLibrary(pipelineLibrary),
    renderpassManager(renderpassManager),
    commandManager(commandManager)
{

}

void Nova::Start(){
    shell->Run(this);
}

std::unique_ptr<IRenderLoopClient>  Nova::Builder::Build(){

    return std::make_unique<Nova>(engine,shell,gpu,framebuffersContainer,syncManager,swapchainManager,pipelineLibrary,renderpassManager,commandManager);
    
}

void AllocateBuffers(){
    
}
void Nova::Init() {
   
                    
    
}
void Nova::Load() {

}
void Nova::Update(float deltaTime){

}
void Nova::Render() {

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