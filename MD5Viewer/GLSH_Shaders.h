#ifndef GLSH_SHADERS_H_
#define GLSH_SHADERS_H_

#include <GL/glew.h>
#include <glm/glm.hpp>                      // glm::vec3, glm::vec4, glm::ivec4, glm::mat4, ...
#include <glm/gtc/type_ptr.hpp>             // glm::value_ptr
#include <string>

namespace glsh {

GLuint CompileShader(GLenum shaderType, const std::string& path);
GLuint CompileVertexShader(const std::string& path);
GLuint CompileFragmentShader(const std::string& path);

GLuint BuildShaderProgram(const std::string& vsPath, const std::string& fsPath);

//
// GetActiveShaderUniformLocation
//
//     Returns the location of a named uniform variable in the currently bound shader program
//     This method is ridiculously inefficient.  You should never use it in production code.
//
GLint GetActiveShaderUniformLocation(const std::string& name);

// Set shader uniforms by cached location (fast)

void SetShaderUniform(GLint location, GLfloat scalar);
void SetShaderUniform(GLint location, const glm::vec2& vec);
void SetShaderUniform(GLint location, const glm::vec3& vec);
void SetShaderUniform(GLint location, const glm::vec4& vec);
void SetShaderUniform(GLint location, const glm::mat3& mat);
void SetShaderUniform(GLint location, const glm::mat4& mat);
void SetShaderUniformInt(GLint location, GLint scalar);

// Set shader uniforms by name (slow)

void SetShaderUniform(const std::string& name, GLfloat scalar);
void SetShaderUniform(const std::string& name, const glm::vec2& vec);
void SetShaderUniform(const std::string& name, const glm::vec3& vec);
void SetShaderUniform(const std::string& name, const glm::vec4& vec);
void SetShaderUniform(const std::string& name, const glm::mat3& mat);
void SetShaderUniform(const std::string& name, const glm::mat4& mat);
void SetShaderUniformInt(const std::string& name, GLint scalar);


////////// Some inline function implementations //////////

inline GLuint CompileVertexShader(const std::string& path)
{
    return CompileShader(GL_VERTEX_SHADER, path);
}

inline GLuint CompileFragmentShader(const std::string& path)
{
    return CompileShader(GL_FRAGMENT_SHADER, path);
}

///// Set shader uniforms by location (fast)

inline void SetShaderUniform(GLint location, GLfloat scalar)
{
    glUniform1f(location, scalar);
}

inline void SetShaderUniform(GLint location, const glm::vec2& vec)
{
    glUniform2fv(location, 1, glm::value_ptr(vec));
}

inline void SetShaderUniform(GLint location, const glm::vec3& vec)
{
    glUniform3fv(location, 1, glm::value_ptr(vec));
}

inline void SetShaderUniform(GLint location, const glm::vec4& vec)
{
    glUniform4fv(location, 1, glm::value_ptr(vec));
}

inline void SetShaderUniform(GLint location, const glm::mat3& mat)
{
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

inline void SetShaderUniform(GLint location, const glm::mat4& mat)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

inline void SetShaderUniformInt(GLint location, GLint scalar)
{
    glUniform1i(location, scalar);
}

///// Set shader uniforms by name (slow)

inline void SetShaderUniform(const std::string& name, GLfloat scalar)
{
    SetShaderUniform(GetActiveShaderUniformLocation(name), scalar);
}

inline void SetShaderUniform(const std::string& name, const glm::vec2& vec)
{
    SetShaderUniform(GetActiveShaderUniformLocation(name), vec);
}

inline void SetShaderUniform(const std::string& name, const glm::vec3& vec)
{
    SetShaderUniform(GetActiveShaderUniformLocation(name), vec);
}

inline void SetShaderUniform(const std::string& name, const glm::vec4& vec)
{
    SetShaderUniform(GetActiveShaderUniformLocation(name), vec);
}

inline void SetShaderUniform(const std::string& name, const glm::mat3& mat)
{
    SetShaderUniform(GetActiveShaderUniformLocation(name), mat);
}

inline void SetShaderUniform(const std::string& name, const glm::mat4& mat)
{
    SetShaderUniform(GetActiveShaderUniformLocation(name), mat);
}

inline void SetShaderUniformInt(const std::string& name, GLint scalar)
{
    SetShaderUniformInt(GetActiveShaderUniformLocation(name), scalar);
}

}

#endif
