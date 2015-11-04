#include "GLSH_Mesh.h"

#include <iostream>

namespace glsh {

VertexMesh* CreateMesh(GLenum drawingMode, const void* verts, unsigned numVerts, const VertexFormat& vertexFormat)
{
    // create a vertex array object (VAO)
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    if (!vao) {
        std::cerr << "*** Poop: Failed to create VAO" << std::endl;
        return NULL;
    }

    // bind the VAO (subsequent vertex attribute info will be stored in this VAO)
    glBindVertexArray(vao);

    // create a vertex buffer object (VBO)
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    if (!vbo) {
        std::cerr << "*** Poop: Failed to create VBO" << std::endl;
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &vao);
        return NULL;
    }

    // bind the VBO (subsequent calls that target GL_ARRAY_BUFFER will apply to this VBO)
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // resize and fill the buffer (copy the vertex data into it)
    glBufferData(GL_ARRAY_BUFFER,               // the buffer to resize and fill
                 vertexFormat.getVertexSizeInBytes() * numVerts,  // total size in bytes
                 verts,                         // address of data in RAM
                 GL_STATIC_DRAW);               // buffer usage drawingMode (GL_STATIC_DRAW == read-only == fast drawing)

    // describe how the vertex positions are layed out in the active buffer
    for (unsigned i = 0; i < vertexFormat.numAttribs(); i++) {
        const VertexAttrib& a = vertexFormat.getAttrib(i);
        glVertexAttribPointer(a.index, a.size, a.type, GL_FALSE, a.stride, a.offset);
        glEnableVertexAttribArray(a.index);
    }

    // check for GL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "*** Poop: GL Error in function " __FUNCTION__ " on line " << __LINE__ << ": " << gluErrorString(err) << std::endl;
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &vao);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &vbo);
        return NULL;
    }

    // unbind the VAO, for now
    glBindVertexArray(0);

    // unbind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //
    // all good, create and return a new Mesh object
    //

    return new VertexMesh(vbo, vao, drawingMode, numVerts);
}


IndexedMesh* CreateMesh(GLenum drawingMode, const void* verts, unsigned numVerts, const VertexFormat& vertexFormat, const void* indices, unsigned numIndices, GLenum indexType)
{
    // create a vertex array object (VAO)
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    if (!vao) {
        std::cerr << "*** Poop: Failed to create VAO" << std::endl;
        return NULL;
    }

    // bind the VAO (subsequent vertex attribute info will be stored in this VAO)
    glBindVertexArray(vao);

    // create a vertex buffer object (VBO)
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    if (!vbo) {
        std::cerr << "*** Poop: Failed to create VBO" << std::endl;
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &vao);
        return NULL;
    }

    // bind the VBO (subsequent calls that target GL_ARRAY_BUFFER will apply to this VBO)
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // resize and fill the buffer (copy the vertex data into it)
    glBufferData(GL_ARRAY_BUFFER,               // the buffer to resize and fill
                 vertexFormat.getVertexSizeInBytes() * numVerts,  // total size in bytes
                 verts,                         // address of data in RAM
                 GL_STATIC_DRAW);               // buffer usage drawingMode (GL_STATIC_DRAW == read-only == fast drawing)

    // describe how the vertex positions are layed out in the active buffer
    for (unsigned i = 0; i < vertexFormat.numAttribs(); i++) {
        const VertexAttrib& a = vertexFormat.getAttrib(i);
        glVertexAttribPointer(a.index, a.size, a.type, GL_FALSE, a.stride, a.offset);
        glEnableVertexAttribArray(a.index);
    }

    //
    // Create IBO (index buffer)
    //
    
    GLuint ibo = 0;
    glGenBuffers(1, &ibo);
    if (!ibo) {
        std::cerr << "*** Poop: Failed to create IBO" << std::endl;
        return false;
    }

    // bind the IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // allocate and fill the index buffer (copy the index data into it)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,   // the buffer to resize and fill
                 GetGLTypeSize(indexType) * numIndices,     // total size in bytes
                 indices,                   // address of data in RAM
                 GL_STATIC_DRAW);           // buffer usage drawingMode (GL_STATIC_DRAW == read-only == fast drawing)


    // check for GL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "*** Poop: GL Error in function " __FUNCTION__ " on line " << __LINE__ << ": " << gluErrorString(err) << std::endl;
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &vao);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &ibo);
        return NULL;
    }

    // unbind the VAO, for now
    glBindVertexArray(0);

    // unbind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // unbind the IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //
    // all good, create and return a new Mesh object
    //
    IndexedMesh* mesh = new IndexedMesh(vbo, ibo, vao, drawingMode, indexType, numIndices);

    return mesh;
}

}
