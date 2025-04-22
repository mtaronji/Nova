#include "Nova.hpp"

Nova::Nova(VulkanEngine &engine, 
    Shell& shell, 
    GPU& gpu, 
    FramebufferManager& framebufferManager, 
    SyncManager& syncManager,
    SwapchainManager& swapchainManager,
    PipelineLibrary& pipelineLibrary,
    RenderPassManager& renderpassManager,
    CommandManager& commandManager,
    std::unordered_map<std::string, Shader>& shaders)
    :engine(engine),
    shell(shell),
    gpu(gpu), 
    framebufferManager(framebufferManager),
    syncManager(syncManager),
    swapchainManager(swapchainManager),
    pipelineLibrary(pipelineLibrary),
    renderpassManager(renderpassManager),
    commandManager(commandManager),
    shaders(shaders)
{

}
Nova::Builder::Builder(){
    InitAttachments(this->attachmentInfos);
}
Nova::Builder& Nova::Builder::CreateShell(){
    this->shell = &Shell();
    return *this;
}
Nova::Builder& Nova::Builder::CreateEngine(){
    this->engine = &VulkanEngine(shell->GetWindow());
    return *this;
}
Nova::Builder& Nova::Builder::CreateGPU(){
    this->gpu = &GPU(*this->engine);
    return *this;
}
Nova::Builder& Nova::Builder::CreateSwapchainManager(){

    auto swapchainManager = SwapchainManager::Builder()
                            .SetVulkanEngine(*this->engine)
                            .SetGPU(*this->gpu)
                            .QuerySwapChainSupport()
                            .ChooseSwapSurfaceFormat()
                            .ChooseSwapPresentMode()
                            .ChooseSwapExtent()
                            .Build();
    
}
Nova::Builder& Nova::Builder::CreateRenderPassManager(){
    //create attachments here for now. One day we will read them from a file
    auto renderpassManager = RenderPassManager::Builder()
                             .SetGPU(this->gpu)
                             .SetAttachments(this->attachmentInfos)
                             .Build();
    return *this;
}
Nova::Builder& Nova::Builder::CreateShaders(){
    auto fragmentShader = Shader(*this->gpu,"shaders/identityF.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    auto vertexShader = Shader(*this->gpu,"shaders/identityV.spv",VK_SHADER_STAGE_VERTEX_BIT);
    shaders = {  
        {"frag",fragmentShader},
        {"vert", vertexShader}
    };

    return *this;
}
Nova::Builder& Nova::Builder::CreateSyncManagers(){
    return *this;
}

Nova::Builder& Nova::Builder::CreatePipelineManagers(){

}
Nova Nova::Builder::Build(){

    return Nova(*engine, 
        *shell, 
        *gpu, 
        *framebufferManager, 
        *syncManager,
        *swapchainManager,
        *pipelineLibrary,
        *renderpassManager,
        *commandManager,
         shaders);

}
void Nova::Init() {

    
}
void Nova::Load() {

}
void Nova::Update(float deltaTime = 0.0f) {

}
void Nova::Render() {

}
void Nova::Shutdown() {

}


void Nova::Builder::InitAttachments(std::vector<AttachmentInfo>& attachmentInfos){

    //default color attachment
    //we will add these to files later
    VkAttachmentDescription colorAttachmentDescription{};
    colorAttachmentDescription.format = swapchainManager->GetImageFormat(); // e.g., VK_FORMAT_B8G8R8A8_UNORM
    colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    AttachmentInfo colorAttachmentInfo{};
    colorAttachmentInfo.description = colorAttachmentDescription;
    colorAttachmentInfo.type = AttachmentInfo::Type::Color;
    colorAttachmentInfo.name = "Default Color Attachment";
    attachmentInfos.push_back(colorAttachmentInfo);

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT; // Use a depth format your GPU supports
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    AttachmentInfo depthAttachmentInfo{};
    depthAttachmentInfo.description = colorAttachmentDescription;
    depthAttachmentInfo.type = AttachmentInfo::Type::Color;
    depthAttachmentInfo.name = "Depth Color Attachment";
    attachmentInfos.push_back(depthAttachmentInfo);

}
