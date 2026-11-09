#include <Nova.hpp>
#include <iostream>
#include <cstdlib> // randcolor
#include <ctime>  
#include <unordered_set>

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

#include <iostream>
    
    static_assert(sizeof(VkQueue) == sizeof(void*), "VkQueue should be pointer-sized");
    std::cout << "sizeof(VkSemaphore)=" << sizeof(VkSemaphore) << "\n"
        << "sizeof(VkFence)=" << sizeof(VkFence) << "\n"
        << "sizeof(void*)=" << sizeof(void*) << "\n";

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
        .WithDescriptorSets()
        .WithTextures({"Texturelabs_InkPaint_394S.jpg"})
        .Build();


    app->Init();

    try {
        app->Start();
    }
    catch (const std::exception& e) {

        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}