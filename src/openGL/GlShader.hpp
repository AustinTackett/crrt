
#pragma once

#include <string>
#include <fstream>
#include <cstdio>
#include <unordered_map>

#include <GL/glew.h>

#include "ErrorCodes.hpp"

namespace CRRT { namespace GL {

/*
    OpenGL Shader Data Containers
*/

static constexpr GLuint EMPTY_GL_SHADER = 0;

struct GlShaderConfig
{
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
};

struct GlShader
{
    GLuint shader = EMPTY_GL_SHADER;
    std::unordered_map<std::string, GLint> uniformNameToLocation;
};

/*
    Utility to Convert GLSL Text Files into Strings
*/

static bool ParseFile(std::string filePath, std::string& outString)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::string tempStringLine;
    outString.clear();
    while (std::getline(file, tempStringLine)) {
        outString += tempStringLine;
        outString += '\n';
    }
    return !file.bad();
}

/*
    Binding Texture as OpenGL Active Shader Program
*/

void BindShader(const GlShader& glShader)
{
    if (!glShader.shader) {
        glUseProgram(0);
        return;
    }

    GLint boundShader;
    glGetIntegerv(GL_CURRENT_PROGRAM, &boundShader);
    bool isBound = boundShader == static_cast<GLint>(glShader.shader);
    if (isBound) { return; } 

    glUseProgram(glShader.shader);
}

/*
    Loading and Releasing Shaders to Device
*/

[[nodiscard]] GlError LoadShader(GlShader& shader, const GlShaderConfig& glConfig)
{
    if (shader.shader) return GlError::shaderAlreadyCreated;

    std::string vertexShaderString;
    bool vShaderFileErr = ParseFile(glConfig.vertexShaderPath, vertexShaderString);
    if (!vShaderFileErr) { return GlError::shaderVertexFileReadFailed; }

    std::string fragmentShaderString;
    bool fShaderFileErr = ParseFile(glConfig.fragmentShaderPath, fragmentShaderString);
    if (!fShaderFileErr) { return GlError::shaderFragmentFileReadFailed; }

    GLuint newVertexShader   = glCreateShader(GL_VERTEX_SHADER);
    GLuint newFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    bool shaderCreationSuccess = newVertexShader && newFragmentShader;
    if (!shaderCreationSuccess) {
        if (newVertexShader)   glDeleteShader(newVertexShader);
        if (newFragmentShader) glDeleteShader(newFragmentShader);
        return GlError::shaderCreationFailed;
    }

    const GLchar* vshaderCharArray = vertexShaderString.c_str();
    const GLchar* fshaderCharArray = fragmentShaderString.c_str();
    glShaderSource(newVertexShader,   1, &vshaderCharArray, NULL);
    glShaderSource(newFragmentShader, 1, &fshaderCharArray, NULL);

    GLint vshaderCompileErr;
    const unsigned int vInfoLogLength = 1024;
    char vShaderInfolog[vInfoLogLength] = {};
    glCompileShader(newVertexShader);
    glGetShaderiv(newVertexShader, GL_COMPILE_STATUS, &vshaderCompileErr);
    if (!vshaderCompileErr)
    {
        glGetShaderInfoLog(newVertexShader, vInfoLogLength, NULL, &vShaderInfolog[0]);
        glDeleteShader(newVertexShader);
        glDeleteShader(newFragmentShader);
        return GlError::shaderVertexCompilationFailed;
    }

    GLint fshaderCompileErr;
    const unsigned int fInfoLogLength = 1024;
    char fShaderInfolog[fInfoLogLength] = {};
    glCompileShader(newFragmentShader);
    glGetShaderiv(newFragmentShader, GL_COMPILE_STATUS, &fshaderCompileErr);
    if (!fshaderCompileErr)
    {
        glGetShaderInfoLog(newFragmentShader, fInfoLogLength, NULL, &fShaderInfolog[0]);
        glDeleteShader(newFragmentShader);
        glDeleteShader(newVertexShader);
        return GlError::shaderFragmentCompilationFailed;
    }

    GLint linkProgramErr;
    GLuint newShaderProgram = glCreateProgram();
    shader.shader = newShaderProgram;
    if (!newShaderProgram)
    {
        glDeleteShader(newVertexShader);
        glDeleteShader(newFragmentShader);
        return GlError::shaderProgramCreationFailed;
    }

    const unsigned int programInfoLogLength = 1024;
    char programInfolog[programInfoLogLength] = {};
    glAttachShader(newShaderProgram, newVertexShader);
    glAttachShader(newShaderProgram, newFragmentShader);
    glLinkProgram(newShaderProgram);
    glGetProgramiv(newShaderProgram, GL_LINK_STATUS, &linkProgramErr);
    if (!linkProgramErr)
    {
        glGetProgramInfoLog(newShaderProgram, programInfoLogLength, NULL, &programInfolog[0]);
        glDeleteShader(newVertexShader);
        glDeleteShader(newFragmentShader);
        return GlError::shaderProgramLinkFailed;
    }

    glDetachShader(newShaderProgram, newVertexShader);
    glDetachShader(newShaderProgram, newFragmentShader);
    glDeleteShader(newVertexShader);
    glDeleteShader(newFragmentShader);

    shader.uniformNameToLocation.clear();
    return GlError::glSuccess;
}

void ReleaseShader(GlShader& glShader)
{
    if (!glShader.shader) return;

    glUseProgram(0);
    glDeleteProgram(glShader.shader);
    glShader.shader = EMPTY_GL_SHADER;
    glShader.uniformNameToLocation.clear();
}

/*
    Setting Shader Uniforms
*/

void SetUniform1i(GlShader& glShader, std::string name, GLuint data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniform1i(glShader.uniformNameToLocation.at(name), data);
}
 
}};
