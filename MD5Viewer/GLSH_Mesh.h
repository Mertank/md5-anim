#ifndef GLSH_MESH_H_
#define GLSH_MESH_H_

#include <GL/glew.h>

#include <vector>

#include "GLSH_Vertex.h"

// a macro that casts an integer offset to a pointer
#define GLSH_BUFFER_OFFSET(i) ((void*)(i))

namespace glsh {

//
// An abstract base class for meshes that use a VAO
//
class Mesh {
protected:
    GLuint  mVAO;            // the VAO describes the data sources and format
    GLenum  mDrawingMode;    // geometric primitive type (GL_TRIANGLES, etc.)

    Mesh(GLuint vao, GLenum drawingMode)
        : mVAO(vao)
        , mDrawingMode(drawingMode)
    { }

public:
    virtual ~Mesh()         // polymorphic base classes need a virtual destructor
    {
        if (mVAO) {
            glDeleteVertexArrays(1, &mVAO);
        }
    }

    void draw() const
    {
        glBindVertexArray(mVAO);

        this->drawImpl();

        glBindVertexArray(0);
    }

protected:

    virtual void drawImpl() const = 0;    // subclasses must implement their own draw call(s)
};


//
// A concrete class for meshes that use a VBO and VAO.
//
class VertexMesh : public Mesh {

    GLuint   mVBO;          // the VBO stores vertex attribute data
    GLsizei  mVertexCount;  // number of vertices

public:
    // NOTE: mesh takes ownership of VBO and VAO!
    VertexMesh(GLuint vbo, GLuint vao, GLenum drawingMode, GLsizei vertexCount)
        : Mesh(vao, drawingMode)  // initialize base
        , mVBO(vbo)
        , mVertexCount(vertexCount)
    { }

    virtual ~VertexMesh() override
    {
        if (mVBO) {
            glDeleteBuffers(1, &mVBO);
        }
    }

protected:

    virtual void drawImpl() const override
    {
        glDrawArrays(mDrawingMode, 0, mVertexCount);
    }
};


//
// A concrete class for meshes that use a VBO, IBO, and VAO.
//
class IndexedMesh : public Mesh {

    GLuint  mVBO;        // the VBO stores vertex attribute data
    GLuint  mIBO;        // the IBO stores the indices

    GLsizei mIndexCount; // number of indices
    GLenum  mIndexType;  // index type (GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, ...)

public:
    // NOTE: mesh takes ownership of VBO, IBO, and VAO
    IndexedMesh(GLuint vbo, GLuint ibo, GLuint vao, GLenum drawingMode, GLenum indexType, GLsizei indexCount)
        : Mesh(vao, drawingMode)  // initialize base
        , mVBO(vbo)
        , mIBO(ibo)
        , mIndexCount(indexCount)
        , mIndexType(indexType)
    { }

    virtual ~IndexedMesh() override
    {
        if (mIBO) {
            glDeleteBuffers(1, &mIBO);
        }
        if (mVBO) {
            glDeleteBuffers(1, &mVBO);
        }
    }

protected:

    virtual void drawImpl() const override
    {
        glDrawElements(mDrawingMode, mIndexCount, mIndexType, GLSH_BUFFER_OFFSET(0));
    }
};


//
// A generic function for creating unindexed meshes from an array of vertices.
//
VertexMesh* CreateMesh(GLenum drawingMode,                  // GL_TRIANGLES, etc.
                       const void* vertices,                // pointer to the start of the vertex array
                       unsigned numVertices,                // number of vertices in the array
                       const VertexFormat& vertexFormat);   // vertex format (attributes and their layout)

//
// Some overloads for creating unindexed meshes from vertices of specific types.
//
// This family of overloads is perfect for a generic template function,
// since the only thing that changes is the vertex type.
//
// Note that VertexType needs to be one of the VertexXXX types from Vertex.h,
// such as VertexPosition, VertexPositionColor, VertexPositionNormalTexture, etc.
// These types have a static GetFormat method that the template relies on.
//
/*
VertexMesh* CreateMesh(GLenum drawingMode, const VertexPosition* vertices, unsigned numVertices);
VertexMesh* CreateMesh(GLenum drawingMode, const VertexPositionColor* vertices, unsigned numVertices);
VertexMesh* CreateMesh(GLenum drawingMode, const VertexPositionTexture* vertices, unsigned numVertices);
VertexMesh* CreateMesh(GLenum drawingMode, const VertexPositionNormal* vertices, unsigned numVertices);
VertexMesh* CreateMesh(GLenum drawingMode, const VertexPositionNormalTexture* vertices, unsigned numVertices);
*/

template <typename VertexType>
VertexMesh* CreateMesh(GLenum drawingMode, const VertexType* vertices, unsigned numVertices)
{
    return CreateMesh(drawingMode, vertices, numVertices, VertexType::GetFormat());
}

/*
VertexMesh* CreateMesh(GLenum drawingMode, const std::vector<VertexPosition>& vertices);
VertexMesh* CreateMesh(GLenum drawingMode, const std::vector<VertexPositionColor>& vertices);
VertexMesh* CreateMesh(GLenum drawingMode, const std::vector<VertexPositionTexture>& vertices);
VertexMesh* CreateMesh(GLenum drawingMode, const std::vector<VertexPositionNormal>& vertices);
VertexMesh* CreateMesh(GLenum drawingMode, const std::vector<VertexPositionNormalTexture>& vertices);
*/

template <typename VertexType>
VertexMesh* CreateMesh(GLenum drawingMode, const std::vector<VertexType>& vertices)
{
    return CreateMesh(drawingMode, &vertices[0], vertices.size(), VertexType::GetFormat());
}

//
//
// A generic function for creating indexed meshes from an array of vertices and an array of indices.
//
//
IndexedMesh* CreateMesh(GLenum drawingMode,             // GL_TRIANGLES, etc.
                        const void* vertices,           // pointer to first vertex in an array
                        unsigned numVertices,           // number of vertices in the array
                        const VertexFormat& vertexFormat, // vertex format
                        const void* indices,            // pointer to first index in an array
                        unsigned numIndices,            // number of indices in the array
                        GLenum indexType);              // index type (GL_UNSIGNED_SHORT, etc.)

//
// A template that creates a mesh using an array VertexType elements
// and an array of IndexType elements.
// - VertexType needs to be one of the VertexXXX types from Vertex.h,
//   such as VertexPosition, VertexPositionColor, VertexPositionNormalTexture, etc.
//   These types have a static GetFormat method that the template relies on.
// - IndexType should be one of unsigned char, unsigned short, or unsigned int.
//
template <typename VertexType, typename IndexType>
IndexedMesh* CreateMesh(GLenum drawingMode, const VertexType* vertices, unsigned numVertices, const IndexType* indices, unsigned numIndices)
{
    // figure out GL type id based on size of IndexType
    GLenum indexType = GL_NONE;
    switch (sizeof(IndexType)) {
    case 1: indexType = GL_UNSIGNED_BYTE; break;
    case 2: indexType = GL_UNSIGNED_SHORT; break;
    case 4: indexType = GL_UNSIGNED_INT; break;
    default: return NULL; // error, invalid index size
    }
    return CreateMesh(drawingMode, vertices, numVertices, VertexType::GetFormat(), indices, numIndices, indexType);
}

template <typename VertexType, typename IndexType>
IndexedMesh* CreateMesh(GLenum drawingMode, const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices)
{
    return CreateMesh(drawingMode, &vertices[0], vertices.size(), &indices[0], indices.size());
}

//
// Draw immediate geometry (from RAM)
//

template <typename VertexType>
void DrawGeometry(GLenum drawingMode, const VertexType* verts, unsigned numVerts)
{
    // unbind any active VAO
    glBindVertexArray(0);

    const VertexFormat& fmt = VertexType::GetFormat();

    const char* ptr = (const char*)verts;  // need a byte-sized pointer for offset arithmetic to work properly

    for (unsigned i = 0; i < fmt.numAttribs(); i++) {
        const VertexAttrib& a = fmt.getAttrib(i);
        glVertexAttribPointer(a.index, a.size, a.type, GL_FALSE, a.stride, ptr + (size_t)a.offset);  // I... fucking... love... this... language
        glEnableVertexAttribArray(a.index);
    }

    glDrawArrays(drawingMode, 0, numVerts);

    for (unsigned i = 0; i < fmt.numAttribs(); i++) {
        const VertexAttrib& a = fmt.getAttrib(i);
        glDisableVertexAttribArray(a.index);
    }
}

template <typename VertexType>
void DrawGeometry(GLenum drawingMode, const std::vector<VertexType>& verts)
{
    DrawGeometry(drawingMode, verts.data(), verts.size());
}

}

#endif
