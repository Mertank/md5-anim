#ifndef GLSH_MATH_H_
#define GLSH_MATH_H_

// gl math
#include <glm/glm.hpp>                      // glm::vec3, glm::vec4, glm::ivec4, glm::mat4, ...
#include <glm/gtc/matrix_transform.hpp>     // glm::translate, glm::rotate, glm::scale, glm::perspective, ...
#include <glm/gtc/type_ptr.hpp>             // glm::value_ptr
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


namespace glsh {

//
// Useful constants
//

const float PI =        3.14159265359f;
const float TWO_PI =    2.0f * PI;
const float HALF_PI =   0.5f * PI;

//
// Transformation matrices
//

inline glm::mat4 CreateTranslation(float x, float y, float z)
{
    glm::mat4 m(1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                x, y, z, 1);
    return m;
}

inline glm::mat4 CreateTranslation(const glm::vec3& t)
{
    glm::mat4 m(  1,   0,   0, 0,
                  0,   1,   0, 0,
                  0,   0,   1, 0,
                t.x, t.y, t.z, 1);
    return m;
}

inline glm::mat4 CreateScale(float s)
{
    glm::mat4 m(s, 0, 0, 0,
                0, s, 0, 0,
                0, 0, s, 0,
                0, 0, 0, 1);
    return m;
}

inline glm::mat4 CreateScale(float sx, float sy, float sz)
{
    glm::mat4 m(sx,  0,  0, 0,
                 0, sy,  0, 0,
                 0,  0, sz, 0,
                 0,  0,  0, 1);
    return m;
}

inline glm::mat4 CreateScale(const glm::vec3& s)
{
    glm::mat4 m(s.x,   0,   0, 0,
                  0, s.y,   0, 0,
                  0,   0, s.z, 0,
                  0,   0,   0, 1);
    return m;
}

inline glm::mat4 CreateRotationZ(float radians)
{
    float s = std::sin(radians);
    float c = std::cos(radians);

    glm::mat4 m( c, s, 0, 0,
                -s, c, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1);
    return m;
}

inline glm::mat4 CreateRotationY(float radians)
{
    float s = std::sin(radians);
    float c = std::cos(radians);

    glm::mat4 m( c,  0, -s, 0,
                 0,  1,  0, 0,
                 s,  0,  c, 0,
                 0,  0,  0, 1);
    return m;
}

inline glm::mat4 CreateRotationX(float radians)
{
    float s = std::sin(radians);
    float c = std::cos(radians);

    glm::mat4 m( 1,  0,  0, 0,
                 0,  c,  s, 0,
                 0, -s,  c, 0,
                 0,  0,  0, 1);
    return m;
}

//
// Quaternion stuff
//

inline glm::quat CreateQuaternion(float radians, const glm::vec3& axis)
{
    // CAUTION: axis cannot be zero-length!
    float len = glm::length(axis);

    // if axis length or angle are (almost) zero, return identity quaternion
    if (len < 0.0001f || std::fabs(radians) < 0.0001f) {
        return glm::quat();  // glm::quat(1.0f, 0.0f, 0.0f, 0.0f)
    } else {
        glm::vec3 v = (1.0f / len) * axis;  // normalized axis
        float c = std::cos(0.5f * radians); // cos of half angle
        float s = std::sin(0.5f * radians); // sin of half angle
        return glm::quat(c, s * v.x, s * v.y, s * v.z);
    }
}

//
// Useful utilities
//

// convert color from bytes (0-255) to floats (0.0-1.0)
inline glm::vec4 NormalizeColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
{
    const float s = 1.0f / 255.0f;
    return glm::vec4(s * r, s * g, s * b, s * a);
}

}

#endif
