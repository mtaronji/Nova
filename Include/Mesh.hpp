#pragma once

#include "BufferOps.hpp"
#include "BufferResource.hpp"
#include "Vertex.hpp"
#include <vector>
class CommandManager;

struct BoundingBox {
    glm::vec3 min;  // Minimum corner (lowest x, y, z)
    glm::vec3 max;  // Maximum corner (highest x, y, z)

    BoundingBox() 
        : min(glm::vec3(FLT_MAX)), max(glm::vec3(-FLT_MAX)) {}

    BoundingBox(const glm::vec3& minPoint, const glm::vec3& maxPoint)
        : min(minPoint), max(maxPoint) {}

    // Expand bounding box to include a point
    void Expand(const glm::vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }

    // Transform the bounding box with a matrix
    BoundingBox Transform(const glm::mat4& matrix) const {
        glm::vec3 corners[8] = {
            {min.x, min.y, min.z}, {max.x, min.y, min.z},
            {min.x, max.y, min.z}, {max.x, max.y, min.z},
            {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {min.x, max.y, max.z}, {max.x, max.y, max.z}
        };

        BoundingBox result;
        for (int i = 0; i < 8; ++i) {
            glm::vec4 transformed = matrix * glm::vec4(corners[i], 1.0f);
            result.Expand(glm::vec3(transformed));
        }
        return result;
    }

    // Center point of the box
    glm::vec3 Center() const {
        return (min + max) * 0.5f;
    }

    // Size (extent) of the box
    glm::vec3 Extents() const {
        return max - min;
    }

};

struct Mesh {
    Mesh() = delete;
    Mesh& operator=(const Mesh&) = delete;
    ~Mesh(){
       
    }

    static Mesh* Create(BufferResource* vertices, BufferResource* indices){
        return new Mesh(vertices, indices);
    }
    void Cleanup(GPU *gpu){
        if(vertexResource != nullptr){vertexResource->Cleanup(gpu); vertexResource = nullptr;}
        if(indiceResource != nullptr){indiceResource->Cleanup(gpu); indiceResource = nullptr;}
    }

    void CreateGPUResources(GPU* gpu, CommandManager* commandManager){
        BufferOps::EnsureDeviceBuffer(*gpu, *commandManager, vertexResource->GetData(), vertexResource->GetDataSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexResource->GetBuffer(), vertexResource->GetMemory(),verticeOffset, 0);
        BufferOps::EnsureDeviceBuffer(*gpu, *commandManager, indiceResource->GetData(), indiceResource->GetDataSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indiceResource->GetBuffer(), indiceResource->GetMemory(), indiceOffset, 0);
    }

    void Bind(VkCommandBuffer cmdBuffer){

    }
    void Draw(VkCommandBuffer){
        
    }

    static std::vector<VertexPC> GenerateCubeVertices(bool solidColor = false);
    static std::vector<uint16_t> GetCubeIndices();
    

    
    glm::mat4 modelMatrix;
    BoundingBox aabb;
    BufferResource* vertexResource = nullptr;
    BufferResource* indiceResource = nullptr;
    bool visible = true;

    protected:
        Mesh(BufferResource* vertices, BufferResource* indices){this->vertexResource = vertices; this->indiceResource = indices;}
    private:
        float verticeOffset = 0.0f;
        float indiceOffset = 0.0f;
};

