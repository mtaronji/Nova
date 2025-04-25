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
    std::shared_ptr<CommandManager> commandManager,
    std::unordered_map<std::string, Shader>& shaders)
    :engine(engine),
    shell(shell),
    gpu(gpu), 
    framebuffersContainer(framebuffersContainer),
    syncManager(syncManager),
    swapchainManager(swapchainManager),
    pipelineLibrary(pipelineLibrary),
    renderpassManager(renderpassManager),
    commandManager(commandManager),
    shaders(shaders)
{

}

void Nova::Start(){
    shell->Run(this);
}
Nova::Builder::Builder(){
    //Load our configs
    RenderPassLoader::LoadFromFile("RenderPasses/defaultRenderpassConfig.json");
}

std::unique_ptr<IRenderLoopClient>  Nova::Builder::Build(){

    return std::make_unique<Nova>(engine,shell,gpu,framebuffersContainer,syncManager,swapchainManager,pipelineLibrary,renderpassManager,commandManager,shaders);
    
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


void Nova::Builder::InitAttachments(std::vector<AttachmentInfo>& attachmentInfos){

   
    
}
