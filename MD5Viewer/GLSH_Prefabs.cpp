#include "GLSH_Prefabs.h"

namespace glsh {

Mesh* CreateWireframeBox(float width, float height, float depth, const glm::mat4& transform)
{
    float x1 = -0.5f * width;
    float x2 =  0.5f * width ;
    float y1 = -0.5f * height;
    float y2 =  0.5f * height;
    float z1 = -0.5f * depth;
    float z2 =  0.5f * depth;

    VertexPosition verts[] = {
        VertexPosition(x1, y1, z1),  // 0 (left-bottom-back)
        VertexPosition(x1, y1, z2),  // 1 (left-bottom-front)
        VertexPosition(x1, y2, z1),  // 2 (left-top-back)
        VertexPosition(x1, y2, z2),  // 3 (left-top-front)
        VertexPosition(x2, y1, z1),  // 4 (right-bottom-back)
        VertexPosition(x2, y1, z2),  // 5 (right-bottom-front)
        VertexPosition(x2, y2, z1),  // 6 (right-top-back)
        VertexPosition(x2, y2, z2),  // 7 (right-top-front)
    };

    // line segments
    unsigned char inds[] = {
        // back-to-front
        0, 1,
        2, 3,
        4, 5,
        6, 7,
        // bottom-to-top
        0, 2,
        1, 3,
        4, 6,
        5, 7,
        // left-to-right
        0, 4,
        1, 5,
        2, 6,
        3, 7,
    };

    unsigned numVerts = sizeof(verts) / sizeof(verts[0]);
    unsigned numInds = sizeof(inds) / sizeof(inds[0]);

    // transform the vertices using the user-supplied transformation matrix
    TransformPositions(verts, numVerts, transform);

    return CreateMesh(GL_LINES, verts, numVerts, inds, numInds);
}

Mesh* CreateSolidBox(float width, float height, float depth, const glm::mat4& transform)
{
    float x1 = -0.5f * width;
    float x2 =  0.5f * width;
    float y1 = -0.5f * height;
    float y2 =  0.5f * height;
    float z1 = -0.5f * depth;
    float z2 =  0.5f * depth;

    std::vector<VertexPositionNormal> vertices;

    // front face
    vertices.push_back(VertexPositionNormal(x1, y1, z2,  0, 0, 1));
    vertices.push_back(VertexPositionNormal(x2, y1, z2,  0, 0, 1));
    vertices.push_back(VertexPositionNormal(x2, y2, z2,  0, 0, 1));

    vertices.push_back(VertexPositionNormal(x1, y2, z2,  0, 0, 1));
    vertices.push_back(VertexPositionNormal(x1, y1, z2,  0, 0, 1));
    vertices.push_back(VertexPositionNormal(x2, y2, z2,  0, 0, 1));

    // back face
    vertices.push_back(VertexPositionNormal(x1, y1, z1,  0, 0, -1));
    vertices.push_back(VertexPositionNormal(x1, y2, z1,  0, 0, -1));
    vertices.push_back(VertexPositionNormal(x2, y2, z1,  0, 0, -1));

    vertices.push_back(VertexPositionNormal(x2, y1, z1,  0, 0, -1));
    vertices.push_back(VertexPositionNormal(x1, y1, z1,  0, 0, -1));
    vertices.push_back(VertexPositionNormal(x2, y2, z1,  0, 0, -1));

    // right face
    vertices.push_back(VertexPositionNormal(x2, y1, z1,  1, 0, 0));
    vertices.push_back(VertexPositionNormal(x2, y2, z1,  1, 0, 0));
    vertices.push_back(VertexPositionNormal(x2, y2, z2,  1, 0, 0));

    vertices.push_back(VertexPositionNormal(x2, y1, z2,  1, 0, 0));
    vertices.push_back(VertexPositionNormal(x2, y1, z1,  1, 0, 0));
    vertices.push_back(VertexPositionNormal(x2, y2, z2,  1, 0, 0));

    // left face
    vertices.push_back(VertexPositionNormal(x1, y1, z1,  -1, 0, 0));
    vertices.push_back(VertexPositionNormal(x1, y1, z2,  -1, 0, 0));
    vertices.push_back(VertexPositionNormal(x1, y2, z2,  -1, 0, 0));

    vertices.push_back(VertexPositionNormal(x1, y2, z1,  -1, 0, 0));
    vertices.push_back(VertexPositionNormal(x1, y1, z1,  -1, 0, 0));
    vertices.push_back(VertexPositionNormal(x1, y2, z2,  -1, 0, 0));

    // top face
    vertices.push_back(VertexPositionNormal(x1, y2, z1,  0, 1, 0));
    vertices.push_back(VertexPositionNormal(x1, y2, z2,  0, 1, 0));
    vertices.push_back(VertexPositionNormal(x2, y2, z2,  0, 1, 0));

    vertices.push_back(VertexPositionNormal(x2, y2, z1,  0, 1, 0));
    vertices.push_back(VertexPositionNormal(x1, y2, z1,  0, 1, 0));
    vertices.push_back(VertexPositionNormal(x2, y2, z2,  0, 1, 0));

    // bottom face
    vertices.push_back(VertexPositionNormal(x1, y1, z1,  0, -1, 0));
    vertices.push_back(VertexPositionNormal(x2, y1, z1,  0, -1, 0));
    vertices.push_back(VertexPositionNormal(x2, y1, z2,  0, -1, 0));
 
    vertices.push_back(VertexPositionNormal(x1, y1, z2,  0, -1, 0));
    vertices.push_back(VertexPositionNormal(x1, y1, z1,  0, -1, 0));
    vertices.push_back(VertexPositionNormal(x2, y1, z2,  0, -1, 0));

    // transform the vertices using the user-supplied transformation matrix
    TransformPositionsAndNormals(vertices, transform);

    return CreateMesh(GL_TRIANGLES, vertices);
}


Mesh* CreateTexturedBox(float width, float height, float depth, const glm::mat4& transform)
{
    std::vector<VPNT> vertices;

    float w = 0.5f * width;
    float h = 0.5f * width;
    float d = 0.5f * width;

    // front face
    vertices.push_back(VPNT(-w, -h,  d,  0, 0, 1,  0, 0));
    vertices.push_back(VPNT( w, -h,  d,  0, 0, 1,  1, 0));
    vertices.push_back(VPNT( w,  h,  d,  0, 0, 1,  1, 1));

    vertices.push_back(VPNT(-w,  h,  d,  0, 0, 1,  0, 1));
    vertices.push_back(VPNT(-w, -h,  d,  0, 0, 1,  0, 0));
    vertices.push_back(VPNT( w,  h,  d,  0, 0, 1,  1, 1));

    // back face
    vertices.push_back(VPNT(-w, -h, -d,  0, 0, -1,  1, 0));
    vertices.push_back(VPNT(-w,  h, -d,  0, 0, -1,  1, 1));
    vertices.push_back(VPNT( w,  h, -d,  0, 0, -1,  0, 1));

    vertices.push_back(VPNT( w, -h, -d,  0, 0, -1,  0, 0));
    vertices.push_back(VPNT(-w, -h, -d,  0, 0, -1,  1, 0));
    vertices.push_back(VPNT( w,  h, -d,  0, 0, -1,  0, 1));

    // right face
    vertices.push_back(VPNT( w, -h, -d,  1, 0, 0,  1, 0));
    vertices.push_back(VPNT( w,  h, -d,  1, 0, 0,  1, 1));
    vertices.push_back(VPNT( w,  h,  d,  1, 0, 0,  0, 1));

    vertices.push_back(VPNT( w, -h,  d,  1, 0, 0,  0, 0));
    vertices.push_back(VPNT( w, -h, -d,  1, 0, 0,  1, 0));
    vertices.push_back(VPNT( w,  h,  d,  1, 0, 0,  0, 1));

    // left face
    vertices.push_back(VPNT(-w, -h, -d,  -1, 0, 0,  0, 0));
    vertices.push_back(VPNT(-w, -h,  d,  -1, 0, 0,  1, 0));
    vertices.push_back(VPNT(-w,  h,  d,  -1, 0, 0,  1, 1));

    vertices.push_back(VPNT(-w,  h, -d,  -1, 0, 0,  0, 1));
    vertices.push_back(VPNT(-w, -h, -d,  -1, 0, 0,  0, 0));
    vertices.push_back(VPNT(-w,  h,  d,  -1, 0, 0,  1, 1));

    // top face
    vertices.push_back(VPNT(-w,  h, -d,  0, 1, 0,  0, 1));
    vertices.push_back(VPNT(-w,  h,  d,  0, 1, 0,  0, 0));
    vertices.push_back(VPNT( w,  h,  d,  0, 1, 0,  1, 0));

    vertices.push_back(VPNT( w,  h, -d,  0, 1, 0,  1, 1));
    vertices.push_back(VPNT(-w,  h, -d,  0, 1, 0,  0, 1));
    vertices.push_back(VPNT( w,  h,  d,  0, 1, 0,  1, 0));

    // bottom face
    vertices.push_back(VPNT(-w, -h, -d,  0, -1, 0,  0, 0));
    vertices.push_back(VPNT( w, -h, -d,  0, -1, 0,  1, 0));
    vertices.push_back(VPNT( w, -h,  d,  0, -1, 0,  1, 1));
 
    vertices.push_back(VPNT(-w, -h,  d,  0, -1, 0,  0, 1));
    vertices.push_back(VPNT(-w, -h, -d,  0, -1, 0,  0, 0));
    vertices.push_back(VPNT( w, -h,  d,  0, -1, 0,  1, 1));

    // transform the vertices using the user-supplied transformation matrix
    TransformPositionsAndNormals(vertices, transform);

    return CreateMesh(GL_TRIANGLES, vertices);
}


Mesh* CreateWireframeCube(float width, const glm::mat4& transform)
{
    return CreateWireframeBox(width, width, width, transform);
}

Mesh* CreateSolidCube(float width, const glm::mat4& transform)
{
    return CreateSolidBox(width, width, width, transform);
}

Mesh* CreateTexturedCube(float width, const glm::mat4& transform)
{
    return CreateTexturedBox(width, width, width, transform);
}

Mesh* CreateWireframePlane(float xSize, float zSize, int xSegments, int zSegments, const glm::mat4& transform)
{
    // sanity check
    if (xSegments < 1 || zSegments < 1) {
        return NULL;
    }

    float y = 0;
    float x = -0.5f * xSize;
    float z = -0.5f * zSize;

    float x1 = -0.5f * xSize;
    float x2 =  0.5f * xSize;
    float z1 = -0.5f * zSize;
    float z2 =  0.5f * zSize;

    float xSpacing = xSize / xSegments;
    float zSpacing = zSize / zSegments;

    // define world grid mesh data
    std::vector<VertexPosition> verts;
    // add back-to-front segments
    for (int i = 0; i <= xSegments; i++) {
        float x = x1 + i * xSpacing;
        verts.push_back(VertexPosition(x, y, z1));
        verts.push_back(VertexPosition(x, y, z2));
    }
    // add left-to-right segments
    for (int i = 0; i <= zSegments; i++) {
        float z = z1 + i * zSpacing;
        verts.push_back(VertexPosition(x1, y, z));
        verts.push_back(VertexPosition(x2, y, z));
    }

    // transform the vertices using the user-supplied transformation matrix
    TransformPositions(verts, transform);

    return CreateMesh(GL_LINES, verts);
}

Mesh* CreateSolidPlane(float xSize, float zSize, int xSegments, int zSegments, const glm::mat4& transform)
{
    int numVertices = (xSegments + 1) * (zSegments + 1);

    std::vector<VertexPositionNormal> vertexData(numVertices);

    float xStep = xSize / xSegments;
    float zStep = zSize / zSegments;

    VertexPositionNormal* vertexPtr = &vertexData[0];

    float z = -0.5f * zSize;
    for (int j = 0; j <= zSegments; j++) {
        float x = -0.5f * xSize;
        for (int i = 0; i <= xSegments; i++) {
            vertexPtr->pos.x = x;
            vertexPtr->pos.y = 0;
            vertexPtr->pos.z = z;
            vertexPtr->normal.x = 0;
            vertexPtr->normal.y = 1;
            vertexPtr->normal.z = 0;
            ++vertexPtr;
            x += xStep;
        }
        z += zStep;
    }

    int numTriangles = 2 * xSegments * zSegments;

    int numElements = 3 * numTriangles;
    std::vector<unsigned> indexData(numElements);   // use 32-bit indices for large planes!!!11!!!

    unsigned* indexPtr = &indexData[0];
    unsigned e = 0;
    for (int j = 0; j < zSegments; j++) {
        for (int i = 0; i < xSegments; i++) {
            // the four corners of this "square"
            unsigned e = (xSegments + 1) * j + i;
            unsigned e1 = e;
            unsigned e2 = e + 1;
            unsigned e3 = e + xSegments + 1;
            unsigned e4 = e + xSegments + 2;
            // triangle 1
            *indexPtr++ = e1;
            *indexPtr++ = e3;
            *indexPtr++ = e4;
            // triangle 2
            *indexPtr++ = e1;
            *indexPtr++ = e4;
            *indexPtr++ = e2;
            ++e;
        }
        ++e;
    }

    return CreateMesh(GL_TRIANGLES, vertexData, indexData);
}


Mesh* CreateHalfAxes(float length, const glm::mat4& transform)
{
    VertexPositionColor verts[] = {
        VertexPositionColor(  0.0f,   0.0f,   0.0f,   0.0f, 0.0f, 1.0f, 1.0f),
        VertexPositionColor(length,   0.0f,   0.0f,   0.0f, 0.0f, 1.0f, 1.0f),
        VertexPositionColor(  0.0f,   0.0f,   0.0f,   0.0f, 1.0f, 0.0f, 1.0f),
        VertexPositionColor(  0.0f, length,   0.0f,   0.0f, 1.0f, 0.0f, 1.0f),
        VertexPositionColor(  0.0f,   0.0f,   0.0f,   1.0f, 0.0f, 0.0f, 1.0f),
        VertexPositionColor(  0.0f,   0.0f, length,   1.0f, 0.0f, 0.0f, 1.0f),
    };

    unsigned numVerts = sizeof(verts) / sizeof(verts[0]);

    // transform the vertices using the user-supplied transformation matrix
    TransformPositions(verts, numVerts, transform);

    return CreateMesh(GL_LINES, verts, numVerts);
}

Mesh* CreateFullAxes(float length, const glm::mat4& transform)
{
    VertexPositionColor verts[] = {
        VertexPositionColor(-length,   0.0f,     0.0f,   0.0f, 0.0f, 1.0f, 1.0f),
        VertexPositionColor( length,   0.0f,     0.0f,   0.0f, 0.0f, 1.0f, 1.0f),
        VertexPositionColor(   0.0f, -length,    0.0f,   0.0f, 1.0f, 0.0f, 1.0f),
        VertexPositionColor(   0.0f,  length,    0.0f,   0.0f, 1.0f, 0.0f, 1.0f),
        VertexPositionColor(   0.0f,    0.0f, -length,   1.0f, 0.0f, 0.0f, 1.0f),
        VertexPositionColor(   0.0f,    0.0f,  length,   1.0f, 0.0f, 0.0f, 1.0f),
    };

    unsigned numVerts = sizeof(verts) / sizeof(verts[0]);

    // transform the vertices using the user-supplied transformation matrix
    TransformPositions(verts, numVerts, transform);

    return CreateMesh(GL_LINES, verts, numVerts);
}

Mesh* CreateChunkyCylinder(float radius, float height, int numSegments, const glm::mat4& transform)
{
    std::vector<VertexPositionNormal> vertices;

    float top = 0.5f * height;
    float bottom = -0.5f * height;

    float angStep = 2 * 3.14159f / numSegments;
    float angle = 0;
    for (int i = 0; i < numSegments; i++) {
        float x1 = radius * std::cos(angle);
        float z1 = radius * std::sin(angle);
        float x2 = radius * std::cos(angle + angStep);
        float z2 = radius * std::sin(angle + angStep);

        // triangle at the base
        vertices.push_back(VertexPositionNormal(0, bottom, 0,    0, -1, 0));
        vertices.push_back(VertexPositionNormal(x1, bottom, z1,  0, -1, 0));
        vertices.push_back(VertexPositionNormal(x2, bottom, z2,  0, -1, 0));

        // triangle at the top
        vertices.push_back(VertexPositionNormal(0, top, 0,    0, 1, 0));
        vertices.push_back(VertexPositionNormal(x2, top, z2,  0, 1, 0));
        vertices.push_back(VertexPositionNormal(x1, top, z1,  0, 1, 0));

        // compute normal for triangles on the side
        glm::vec3 a(x1, top, z1);
        glm::vec3 b(x1, bottom, z1);
        glm::vec3 c(x2, bottom, z2);
        glm::vec3 u = b - a;
        glm::vec3 v = c - a;
        glm::vec3 n = glm::normalize(glm::cross(v, u));

        vertices.push_back(VertexPositionNormal(x1, top, z1,     n.x, n.y, n.z));
        vertices.push_back(VertexPositionNormal(x2, bottom, z2,  n.x, n.y, n.z));
        vertices.push_back(VertexPositionNormal(x1, bottom, z1,  n.x, n.y, n.z));

        vertices.push_back(VertexPositionNormal(x1, top, z1,     n.x, n.y, n.z));
        vertices.push_back(VertexPositionNormal(x2, top, z2,     n.x, n.y, n.z));
        vertices.push_back(VertexPositionNormal(x2, bottom, z2,  n.x, n.y, n.z));

        angle += angStep;
    }

    // transform the vertices using the user-supplied transformation matrix
    TransformPositionsAndNormals(vertices, transform);

    return CreateMesh(GL_TRIANGLES, vertices);
}

Mesh* CreateSmoothCylinder(float radius, float height, int numSegments, const glm::mat4& transform)
{
    std::vector<VertexPositionNormal> vertices;

    float top = 0.5f * height;
    float bottom = -0.5f * height;

    float angStep = 2 * 3.14159f / numSegments;
    float angle = 0;
    for (int i = 0; i < numSegments; i++) {
        float x1 = radius * std::cos(angle);
        float z1 = radius * std::sin(angle);
        float x2 = radius * std::cos(angle + angStep);
        float z2 = radius * std::sin(angle + angStep);

        // triangle at the base
        vertices.push_back(VertexPositionNormal(0, bottom, 0,    0, -1, 0));
        vertices.push_back(VertexPositionNormal(x1, bottom, z1,  0, -1, 0));
        vertices.push_back(VertexPositionNormal(x2, bottom, z2,  0, -1, 0));

        // triangle at the top
        vertices.push_back(VertexPositionNormal(0, top, 0,    0, 1, 0));
        vertices.push_back(VertexPositionNormal(x2, top, z2,  0, 1, 0));
        vertices.push_back(VertexPositionNormal(x1, top, z1,  0, 1, 0));

        // compute normals for triangles on the side
        glm::vec3 n1 = glm::normalize(glm::vec3(x1, 0.0f, z1));
        glm::vec3 n2 = glm::normalize(glm::vec3(x2, 0.0f, z2));

        vertices.push_back(VertexPositionNormal(x1, top, z1,     n1.x, n1.y, n1.z));
        vertices.push_back(VertexPositionNormal(x2, bottom, z2,  n2.x, n2.y, n2.z));
        vertices.push_back(VertexPositionNormal(x1, bottom, z1,  n1.x, n1.y, n1.z));

        vertices.push_back(VertexPositionNormal(x1, top, z1,     n1.x, n1.y, n1.z));
        vertices.push_back(VertexPositionNormal(x2, top, z2,     n2.x, n2.y, n2.z));
        vertices.push_back(VertexPositionNormal(x2, bottom, z2,  n2.x, n2.y, n2.z));

        angle += angStep;
    }

    // transform the vertices using the user-supplied transformation matrix
    TransformPositionsAndNormals(vertices, transform);

    return CreateMesh(GL_TRIANGLES, vertices);
}

Mesh* CreateChunkyCone(float radius, float height, int numSegments, const glm::mat4& transform)
{
    std::vector<VertexPositionNormal> vertices;

    float top = 0.5f * height;
    float bottom = -0.5f * height;

    float angStep = 2 * 3.14159f / numSegments;
    float angle = 0;
    for (int i = 0; i < numSegments; i++) {
        float x1 = radius * std::cos(angle);
        float z1 = radius * std::sin(angle);
        float x2 = radius * std::cos(angle + angStep);
        float z2 = radius * std::sin(angle + angStep);

        // triangle at the base
        vertices.push_back(VertexPositionNormal(0, bottom, 0,    0, -1, 0));
        vertices.push_back(VertexPositionNormal(x1, bottom, z1,  0, -1, 0));
        vertices.push_back(VertexPositionNormal(x2, bottom, z2,  0, -1, 0));

        // compute normal for triangle on the side
        glm::vec3 a(0, top, 0);
        glm::vec3 b(x1, bottom, z1);
        glm::vec3 c(x2, bottom, z2);
        glm::vec3 u = b - a;
        glm::vec3 v = c - a;
        glm::vec3 n = glm::normalize(glm::cross(v, u));

        // triangle on the side
        vertices.push_back(VertexPositionNormal(0, top, 0,       n.x, n.y, n.z));
        vertices.push_back(VertexPositionNormal(x2, bottom, z2,  n.x, n.y, n.z));
        vertices.push_back(VertexPositionNormal(x1, bottom, z1,  n.x, n.y, n.z));

        angle += angStep;
    }

    // transform the vertices using the user-supplied transformation matrix
    TransformPositionsAndNormals(vertices, transform);

    return CreateMesh(GL_TRIANGLES, vertices);
}


} // end of namespace
