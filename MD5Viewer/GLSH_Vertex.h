#ifndef GLSH_VERTEX_H_
#define GLSH_VERTEX_H_

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

namespace glsh {

//
// Vertex attribute locations.
// All vertex shaders must follow this convention (use layout(location=X) qualifiers)
//
enum VertexAttribIndex {
    VA_POSITION  = 0,
    VA_COLOR     = 1,
    VA_NORMAL    = 2,
    VA_TEXCOORD  = 3,
};

//
// Useful utilities
//
GLsizei GetGLTypeSize(GLenum type);  // get the size of GL_FLOAT, GL_INT, GL_UNSIGNED_SHORT, etc.

//
// A structure that holds the arguments to glVertexAttribPointer.
// In essence, it defines the layout of a vertex attribute in a vertex array or a VBO.
//
struct VertexAttrib {
    GLuint          index;
    GLint           size;   // number of components
    GLenum          type;
    GLsizei         stride;
    const GLvoid*   offset;

    // default constructor initializes everything to 0 (meaningless values)
    VertexAttrib()
        : index(0), size(0), type(0), stride(0), offset(0)
    { }

    VertexAttrib(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* offset)
        : index(index), size(size), type(type), stride(stride), offset(offset)
    { }

    GLsizei getSizeInBytes() const
    { return size * GetGLTypeSize(type); }  // number of components times component size
};


//
// A VertexFormat is defined as a list of VertexAttributes
//
class VertexFormat {
    std::vector<VertexAttrib>   mAttribs;   // list of attributes
    GLsizei                     mSize;      // total size in bytes

public:
    // default constructor (creates empty attribute list)
    VertexFormat();

    // constructors for easily creating VertexFormats with a few attributes
    VertexFormat(const VertexAttrib& va);
    VertexFormat(const VertexAttrib& va1, const VertexAttrib& va2);
    VertexFormat(const VertexAttrib& va1, const VertexAttrib& va2, const VertexAttrib& va3);
    VertexFormat(const VertexAttrib& va1, const VertexAttrib& va2, const VertexAttrib& va3, const VertexAttrib& va4);

    void addAttrib(const VertexAttrib& va)
    {
        mAttribs.push_back(va);
        mSize += va.getSizeInBytes();
    }

    unsigned numAttribs() const
    { return mAttribs.size(); }

    const VertexAttrib& getAttrib(unsigned i) const
    { return mAttribs[i]; }

    GLsizei getVertexSizeInBytes() const
    { return mSize; }
};


//
// a structure that stores vertex positions
//
struct VertexPosition {

    // the following floats will be laid out consecutively in memory like this: { pos.x, pos.y, pos.z }
    glm::vec3 pos;

    // default constructor initializes all members to 0 (except alpha, which defaults to 1)
    VertexPosition()
        : pos(0.0f, 0.0f, 0.0f)
    { }

    // fully parameterized constructor initializes all members from arguments
    VertexPosition(GLfloat x, GLfloat y, GLfloat z)
        : pos(x, y, z)
    { }

    static const VertexFormat& GetFormat();
};

//
// a structure that stores vertex position and color
//
struct VertexPositionColor {

    // the following floats will be laid out consecutively in memory like this: { pos.x, pos.y, pos.z, color.r, color.g, color.b, color.a }
    glm::vec3 pos;
    glm::vec4 color;

    // default constructor initializes all members to 0 (except alpha, which defaults to 1)
    VertexPositionColor()
        : pos(0.0f, 0.0f, 0.0f)
        , color(0.0f, 0.0f, 0.0f, 1.0f)
    { }

    // fully parameterized constructor initializes all members from arguments
    VertexPositionColor(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
        : pos(x, y, z)
        , color(r, g, b, a)
    { }

    static const VertexFormat& GetFormat();
};


//
// a structure that stores vertex position and texture coordinates
//
struct VertexPositionTexture {

    // the following floats will be laid out consecutively in memory like this: { pos.x, pos.y, pos.z, texcoord.u, texcoord.v }
    glm::vec3 pos;
    glm::vec2 texcoord;

    // default constructor initializes all members to 0
    VertexPositionTexture()
        : pos(0.0f, 0.0f, 0.0f)
        , texcoord(0.0f, 0.0f)
    { }

    // fully parameterized constructor initializes all members from arguments
    VertexPositionTexture(GLfloat x, GLfloat y, GLfloat z, GLfloat u, GLfloat v)
        : pos(x, y, z)
        , texcoord(u, v)
    { }

    static const VertexFormat& GetFormat();
};

//
// a structure that stores vertex position and normal
//
struct VertexPositionNormal {

    // the following floats will be laid out consecutively in memory like this: { pos.x, pos.y, pos.z, normal.x, normal.y, normal.z }
    glm::vec3 pos;
    glm::vec3 normal;

    // default constructor initializes all members to 0 (CAUTION: this creates an invalid normal!)
    VertexPositionNormal()
        : pos(0.0f, 0.0f, 0.0f)
        , normal(0.0f, 0.0f, 0.0f)
    { }

    // fully parameterized constructor initializes all members from arguments
    VertexPositionNormal(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz)
        : pos(x, y, z)
        , normal(nx, ny, nz)
    { }

    static const VertexFormat& GetFormat();
};

//
// a structure that stores vertex position, normal, and texture coordinates
//
struct VertexPositionNormalTexture {

    // the following floats will be laid out consecutively in memory like this: { pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, texcoord.u, texcoord.v }
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texcoord;

    // default constructor initializes all members to 0 (CAUTION: this creates an invalid normal!)
    VertexPositionNormalTexture()
        : pos(0.0f, 0.0f, 0.0f)
        , normal(0.0f, 0.0f, 0.0f)
        , texcoord(0.0f, 0.0f)
    { }

    // fully parameterized constructor initializes all members from arguments
    VertexPositionNormalTexture(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat u, GLfloat v)
        : pos(x, y, z)
        , normal(nx, ny, nz)
        , texcoord(u, v)
    { }

    static const VertexFormat& GetFormat();
};

//
// Short aliases for vertex types (saves some typing and horizontal space)
//

typedef VertexPosition                  VP;
typedef VertexPositionColor             VPC;
typedef VertexPositionNormal            VPN;
typedef VertexPositionTexture           VPT;
typedef VertexPositionNormalTexture     VPNT;

}

#endif
