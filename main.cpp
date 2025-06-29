#include <Nova.hpp>
#include <iostream>
#include <cstdlib> // randcolor
#include <ctime>  

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    std::unique_ptr<IRenderLoopClient> app = Nova::Builder()
                                            .WithShell()
                                            .WithEngine()
                                            .WithGPU()
                                            .WithSwapchainManager()
                                            .WithRenderpassLibrary()                             
                                            .WithDescriptorAllocator()
                                            .WithPipelineLibrary()
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