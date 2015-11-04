#include "GLSH_Shaders.h"
#include "GLSH_Util.h"

#include <iostream>
#include <map>
#include <set>

namespace glsh {

GLuint CompileShader(GLenum shaderType, const std::string& path)
{
    // load shader source code from a text file
    std::string source = glsh::ReadTextFile(path);

    // create shader object of the appropriate type
    GLuint so = glCreateShader(shaderType);
    if (!so) {
        std::cerr << "*** Poop: Failed to create shader object" << std::endl;
        return GL_NONE;
    }

    // attach shader source code
    const char* cSource = source.c_str();
    glShaderSource(so, 1, &cSource, NULL);

    // compile the shader
    glCompileShader(so);

    // check result of compilation
    GLint result;
    glGetShaderiv(so, GL_COMPILE_STATUS, &result);
    if (!result) {
        std::cerr << "*** Poop: failed to compile shader " << path << ":\n";

        GLint infoLogLength = 0;
        glGetShaderiv(so, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        if (infoLogLength > 0) {
            char* infoLog = new char[infoLogLength];
            glGetShaderInfoLog(so, infoLogLength, NULL, infoLog);
            std::cerr << infoLog << std::endl;
            delete [] infoLog;
        }

        return GL_NONE;
    }

    // return shader object id
    return so;
}

GLuint BuildShaderProgram(const std::string& vsPath, const std::string& fsPath)
{
    // load and compile vertex shader
    GLuint vs = CompileVertexShader(vsPath);
    if (!vs)
        return GL_NONE;

    // load and compile fragment shader
    GLuint fs = CompileFragmentShader(fsPath);
    if (!fs) {
        glDeleteShader(vs);   // cleanup
        return GL_NONE;
    }

    // create shader program object
    GLuint prog = glCreateProgram();
    if (!prog) {
        std::cerr << "*** Poop: Failed to create program object" << std::endl;
        glDeleteShader(vs);
        glDeleteShader(fs);
        return GL_NONE;
    }

    // attach vertex and fragment shaders to the program object
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);

    // link program
    glLinkProgram(prog);

    // shader objects no longer needed once program is linked
    glDeleteShader(vs);
    glDeleteShader(fs);

    // check link status
    GLint linkStatus;
    glGetProgramiv(prog, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus) {
        std::cerr << "*** Poop: Failed to vertex shader " << vsPath << " with fragment shader " << fsPath << std::endl;
        glDeleteProgram(prog);
        return GL_NONE;
    }

    // check for GL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "*** Poop: GL Error in function " __FUNCTION__ " on line " << __LINE__ << ": " << gluErrorString(err) << std::endl;
        glDeleteProgram(prog);
        return GL_NONE;
    }

    // return program id
    return prog;
}


GLint GetActiveShaderUniformLocation(const std::string& name)
{
    // first, get the currently active program id
    GLint prog = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
    if (!prog) {
        std::cerr << "*** Can't get shader uniform location - no active program" << std::endl;
        return -1;
    }

    // next, get the location of the named uniform in the currently active program
    GLint loc = glGetUniformLocation(prog, name.c_str());
    if (loc < 0) {
        // only report missing name errors once
        static std::map<GLint, std::set<std::string>> missingUniforms;
        if (missingUniforms[prog].find(name) == missingUniforms[prog].end()) {
            std::cerr << "*** Currently active program does not have a uniform named '" << name << "'" << std::endl;
            missingUniforms[prog].insert(name);
        }
        return loc;
    }

    return loc;
}

}
