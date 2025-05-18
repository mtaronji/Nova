#include "Nova.hpp"
#include <iostream>

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    std::unique_ptr<IRenderLoopClient> app = Nova::Builder()
                                            .WithShell()
                                            .WithEngine()
                                            .WithGPU()
                                            .WithSwapchainManager()
                                            .WithRenderpass()
                                            .WithFramebufferGenerator()
                                            .WithPipelineLibrary()
                                            .WithDescriptorAllocator()
                                            .WithPipelineManager()
                                            .WithSyncManager()
                                            .WithCommandManager()
                                            .WithResourceManager()
                                            .WithMeshes()
                                            .WithResourceMap()
                                            .WithDescriptorSets()
                                            .Build();
                                            
                                            
    app->Init();
                            
    try {
        app->Start();
    } catch (const std::exception& e) {
    
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}