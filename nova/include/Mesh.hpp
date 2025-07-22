#pragma once

#include "BufferOps.hpp"
#include "BufferResource.hpp"
#include "CommandManager.hpp"
#include "Vertex.hpp"
#include <vector>
#include <memory>

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

    static Mesh Create(std::shared_ptr<BufferResource> vertices, std::shared_ptr<BufferResource>indices, glm::mat4 modelMatrix =  glm::mat4(1.0f)){
        return Mesh(vertices,indices, modelMatrix);
    }
    void Cleanup(GPU *gpu){
    
    }

    void CreateGPUResources(GPU* gpu, CommandManager* commandManager){
        BufferOps::EnsureDeviceBuffer(*gpu, *commandManager, vertexResource->Data, vertexResource->DataSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexResource->Buffer, vertexResource->Memory,verticeOffset, 0);
        BufferOps::EnsureDeviceBuffer(*gpu, *commandManager, indiceResource->Data, indiceResource->DataSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indiceResource->Buffer, indiceResource->Memory, indiceOffset, 0);
    }

    void Bind(VkCommandBuffer cmdBuffer){

    }
    void Draw(VkCommandBuffer){
        
    }

    static std::vector<VertexPC> GenerateCubeVertices(bool solidColor = false);
    static std::vector<VertexPNTC> GenerateCubeVerticesPNTC(bool solidColor = false);
    static std::vector<uint16_t> GetCubeIndices();
    static std::vector<uint16_t> GetCubeIndicesPNTC();
    

    
    glm::mat4 modelMatrix;
    BoundingBox aabb;
    std::shared_ptr<BufferResource> vertexResource;  // represents the vertices
    std::shared_ptr<BufferResource> indiceResource;  // represents the order you draw them in
    bool visible = true;

    protected:
        Mesh(std::shared_ptr<BufferResource> vertices, std::shared_ptr<BufferResource> indices, glm::mat4 modelMatrix):
        vertexResource(std::move(vertices)), indiceResource(std::move(indices)){
    
            this->modelMatrix = modelMatrix; 
        }
    private:
        float verticeOffset = 0.0f;
        float indiceOffset = 0.0f;
};

