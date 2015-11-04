#ifndef GLSH_PREFABS_H_
#define GLSH_PREFABS_H_

#include "GLSH_Mesh.h"
#include "GLSH_Math.h"

namespace glsh {

Mesh* CreateWireframeBox    (float width, float height, float depth, const glm::mat4& transform = glm::mat4(1.0f));
Mesh* CreateSolidBox        (float width, float height, float depth, const glm::mat4& transform = glm::mat4(1.0f));
Mesh* CreateTexturedBox     (float width, float height, float depth, const glm::mat4& transform = glm::mat4(1.0f));

Mesh* CreateWireframeCube   (float width, const glm::mat4& transform = glm::mat4(1.0f));
Mesh* CreateSolidCube       (float width, const glm::mat4& transform = glm::mat4(1.0f));
Mesh* CreateTexturedCube    (float width, const glm::mat4& transform = glm::mat4(1.0f));

Mesh* CreateChunkyCylinder  (float radius, float height, int numSegments, const glm::mat4& transform = glm::mat4(1.0f));
Mesh* CreateSmoothCylinder  (float radius, float height, int numSegments, const glm::mat4& transform = glm::mat4(1.0f));

Mesh* CreateChunkyCone      (float radius, float height, int numSegments, const glm::mat4& transform = glm::mat4(1.0f));

Mesh* CreateWireframePlane  (float xSize, float zSize, int xSegments, int zSegments, const glm::mat4& transform = glm::mat4(1.0f));
Mesh* CreateSolidPlane      (float xSize, float zSize, int xSegments, int zSegments, const glm::mat4& transform = glm::mat4(1.0f));

Mesh* CreateHalfAxes        (float length = 1.0f, const glm::mat4& transform = glm::mat4(1.0f));
Mesh* CreateFullAxes        (float length = 1.0f, const glm::mat4& transform = glm::mat4(1.0f));


//
// Generic functions for transforming vertex positions (and normals) in vertex arrays.
//

template <typename VertexType>
void TransformPositions(VertexType* vertices, unsigned numVertices, const glm::mat4& transform)
{
    for (unsigned i = 0; i < numVertices; i++) {
        glm::vec4 pos = transform * glm::vec4(vertices[i].pos, 1.0f);   // apply transform in homogeneous coordinates
        vertices[i].pos = glm::vec3(pos);                               // set transformed position
    }
}

template <typename VertexType>
void TransformPositions(std::vector<VertexType>& vertices, const glm::mat4& transform)
{
    TransformPositions(&vertices[0], vertices.size(), transform);
}

template <typename VertexType>
void TransformPositionsAndNormals(VertexType* vertices, unsigned numVertices, const glm::mat4& transform)
{
    // the normal matrix discards any scaling and translation (keeps rotation only)
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));

    for (unsigned i = 0; i < numVertices; i++) {
        glm::vec4 pos = transform * glm::vec4(vertices[i].pos, 1.0f);   // apply transform in homogeneous coordinates
        vertices[i].pos = glm::vec3(pos);                               // set transformed position
        vertices[i].normal = normalMatrix * vertices[i].normal;         // set transformed normal
    }
}

template <typename VertexType>
void TransformPositionsAndNormals(std::vector<VertexType>& vertices, const glm::mat4& transform)
{
    TransformPositionsAndNormals(&vertices[0], vertices.size(), transform);
}

} // end of namespace

#endif
