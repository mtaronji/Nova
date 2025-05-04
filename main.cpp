#include "Nova.hpp"
#include <vector>
#include "BufferOps.hpp"
#include "Mesh.hpp"
#include "UBOs.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// std::vector<VertexPC> vertices = {
//     {{-0.5f, -0.5f, 0.5f}, {0.689237f, 0.946210f, 0.467340f, 1.0f}},
//     {{ 0.5f, -0.5f, 0.5f}, {0.205835f, 0.429184f, 0.883887f, 1.0f}},
//     {{ 0.5f,  0.5f, 0.5f}, {0.157999f, 0.274672f, 0.190555f, 1.0f}},
//     {{-0.5f,  0.5f, 0.5f}, {0.830268f, 0.516984f, 0.865717f, 1.0f}},
//     {{-0.5f, -0.5f, 0.5f}, {0.492981f, 0.337636f, 0.483383f, 1.0f}},
//     {{ 0.5f, -0.5f, 0.5f}, {0.144742f, 0.036668f, 0.203769f, 1.0f}},
//     {{ 0.5f,  0.5f, 0.5f}, {0.794876f, 0.363344f, 0.536809f, 1.0f}},
//     {{-0.5f,  0.5f, 0.5f}, {0.937225f, 0.103618f, 0.603995f, 1.0f}}
// };
// // Cube indices
// std::vector<uint32_t> indices = {
//     0, 1, 2, 0, 2, 3,  // Bottom face
//     4, 5, 6, 4, 6, 7,  // Top face
//     0, 1, 5, 0, 5, 4,  // Front face
//     2, 3, 7, 2, 7, 6,  // Back face
//     0, 3, 7, 0, 7, 4,  // Left face
//     1, 2, 6, 1, 6, 5   // Right face
// };

//triangle face
std::vector<VertexPC> vertices = {
    {{-0.5f, -0.5f, 0.5f}, {0.689237f, 0.946210f, 0.467340f, 1.0f}},
    {{ 0.5f, -0.5f, 0.5f}, {0.205835f, 0.429184f, 0.883887f, 1.0f}},
    {{ 0.5f,  0.5f, 0.5f}, {0.157999f, 0.274672f, 0.190555f, 1.0f}}
    // {{-0.5f,  0.5f, 0.5f}, {0.830268f, 0.516984f, 0.865717f, 1.0f}},
    // {{-0.5f, -0.5f, 0.5f}, {0.492981f, 0.337636f, 0.483383f, 1.0f}},
    // {{ 0.5f, -0.5f, 0.5f}, {0.144742f, 0.036668f, 0.203769f, 1.0f}},
    // {{ 0.5f,  0.5f, 0.5f}, {0.794876f, 0.363344f, 0.536809f, 1.0f}},
    // {{-0.5f,  0.5f, 0.5f}, {0.937225f, 0.103618f, 0.603995f, 1.0f}}
};
// Cube indices
std::vector<uint32_t> indices = {
    0, 2, 1
};


int main() {
    float aspectRatio = 800.0f/600.0f;
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);   // behind the cube, 5 units away
    glm::vec3 cameraTarget   = glm::vec3(0.0f, 0.0f, 0.0f);   // looking at center of cube
    glm::vec3 upVector       = glm::vec3(0.0f, 1.0f, 0.0f);   // Y-up world

    CameraUBO ubo;
    ubo.cameraPosition = glm::vec3(0.0f,0.0f,5.0f);
    ubo.proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);  // Perspective projection
    ubo.view = glm::lookAt(cameraPosition, cameraTarget, upVector);  // LookAt view

    auto verticesResource = BufferResource(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    auto indicesResource = BufferResource(VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    auto cameraResource = BufferResource(0,0, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    verticesResource.UpdateData(vertices.data(),vertices.size() * sizeof(VertexPC), vertices.size());
    indicesResource.UpdateData(indices.data(), sizeof(uint32_t) * indices.size(),indices.size());
    cameraResource.UpdateData(& ubo, sizeof(ubo), 0);

    std::unordered_map<std::string, BufferResource> resourceMap = {
        {"vertices", std::move(verticesResource)},
        {"indices",std::move(indicesResource)},
        {"camera" , std::move(cameraResource)}
    };

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
                                            .Build();
                                            
                                            
    app->Init();
    app->LoadResourceMap(&resourceMap);
                            
    try {
        app->Start();
    } catch (const std::exception& e) {
    
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}


// const std::vector<Vertex> square = {
//     {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
//     {{0.5f, -0.5f,0.0f}, {0.0f, 1.0f, 0.0f}},
//     {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
//     {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
// };


// const std::vector<Vertex> box = {
//     {{-0.5f, -0.5f,0.0f}, {1.0f, 0.0f, 0.0f}},  //front face
//     {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},  //
//     {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},  //
//     {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},  //

//      {{-0.5f, -0.5f,0.0f}, {1.0f, 0.0f, 0.0f}},  //back face
//     {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},  //
//     {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},  //
//     {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},  //

// };

    //In vkCmdDrawIndexed, an instance refers to one complete copy of the geometry described by the vertex and index buffers. 
            //When you specify the instanceCount parameter, you are telling Vulkan how many instances of that geometry you want to draw.
            //you will need to specify a new buffer for this which is outlined below
            // Use a Per-Instance Data Buffer
            // Create a separate buffer that contains the position (or transformation) for each instance.
            // Bind this buffer to your pipeline as a vertex attribute or a uniform buffer.
            // For example, if you want to specify just the position of each instance, your data buffer might look like this:

            // cpp
            // Copy code
            // struct InstanceData {
            //     glm::vec3 position; // Position of the instance
            // };
            // InstanceData instanceData[10] = {
            //     {glm::vec3(0.0f, 0.0f, 0.0f)},  // Instance 1
            //     {glm::vec3(2.0f, 0.0f, 0.0f)},  // Instance 2
            //     {glm::vec3(-2.0f, 0.0f, 0.0f)}, // Instance 3
            //     // ... Add remaining positions
            // };