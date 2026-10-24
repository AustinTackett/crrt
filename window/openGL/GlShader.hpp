
#pragma once

#include <string>
#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace GL {

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
    Utility to Convert Text Files into Strings
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
    return true;
}

/*
    Binding Texture as OpenGL Active Shader Program
*/

static void BindShader(const GlShader& glShader)
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

static GlShader LoadShader(GlShaderConfig glConfig)
{
    GlShader shader;

    std::string vertexShaderString;
    bool vShaderFileErr = ParseFile(glConfig.vertexShaderPath, vertexShaderString);
    if (!vShaderFileErr)
    {
        throw std::runtime_error("Could not load vertex shader at path " + glConfig.vertexShaderPath + "\n");
    }

    std::string fragmentShaderString;
    bool fShaderFileErr = ParseFile(glConfig.fragmentShaderPath, fragmentShaderString);
    if (!fShaderFileErr)
    {
        throw std::runtime_error("Could not load fragment shader at path " + glConfig.fragmentShaderPath + "\n");
    }

    GLuint newVertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint newFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* vshaderCharArray = vertexShaderString.c_str();
    const GLchar* fshaderCharArray = fragmentShaderString.c_str();
    glShaderSource(newVertexShader, 1, &vshaderCharArray, NULL);
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
        throw std::runtime_error("Could not compile vertex shaders:\n" + std::string(vShaderInfolog) + "\n");
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
        throw std::runtime_error("Could not compile fragment shaders:\n" + std::string(fShaderInfolog) + "\n");
    }

    GLint linkProgramErr;
    GLuint newShaderProgram = glCreateProgram();
    if (!newShaderProgram)
    {
        glDeleteShader(newVertexShader);
        glDeleteShader(newFragmentShader);
        throw std::runtime_error("Could not create OpenGL program");
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
        glDeleteProgram(newShaderProgram);
        throw std::runtime_error("Could not link opengl program\n" + std::string(programInfolog) + "\n");
    }

    glDetachShader(newShaderProgram, newVertexShader);
    glDetachShader(newShaderProgram, newFragmentShader);
    glDeleteShader(newVertexShader);
    glDeleteShader(newFragmentShader);

    shader.shader = newShaderProgram;
    return shader;
}

static void ReleaseShader(GlShader& glShader)
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

static void SetUniformMat4f(GlShader& glShader, std::string name, glm::mat4 data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniformMatrix4fv(glShader.uniformNameToLocation.at(name), 1, GL_FALSE, &data[0][0]);
}

static void SetUniformMat4x3f(GlShader& glShader, std::string name, glm::mat4x3 data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniformMatrix4x3fv(glShader.uniformNameToLocation.at(name), 1, GL_FALSE, &data[0][0]);
}

static void SetUniformMat4x2f(GlShader& glShader, std::string name, glm::mat4x2 data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniformMatrix4x2fv(glShader.uniformNameToLocation.at(name), 1, GL_FALSE, &data[0][0]);
}

static void SetUniformMat3x4f(GlShader& glShader, std::string name, glm::mat3x4 data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniformMatrix3x4fv(glShader.uniformNameToLocation.at(name), 1, GL_FALSE, &data[0][0]);
}

static void SetUniformMat3f(GlShader& glShader, std::string name, glm::mat3 data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniformMatrix3fv(glShader.uniformNameToLocation.at(name), 1, GL_FALSE, &data[0][0]);
}

static void SetUniformMat3x2f(GlShader& glShader, std::string name, glm::mat3x2 data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniformMatrix3x2fv(glShader.uniformNameToLocation.at(name), 1, GL_FALSE, &data[0][0]);
}

static void SetUniformMat2x4f(GlShader& glShader, std::string name, glm::mat2x4 data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniformMatrix2x4fv(glShader.uniformNameToLocation.at(name), 1, GL_FALSE, &data[0][0]);
}

static void SetUniformMat2x3f(GlShader& glShader, std::string name, glm::mat2x3 data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniformMatrix2x3fv(glShader.uniformNameToLocation.at(name), 1, GL_FALSE, &data[0][0]);
}

static void SetUniformMat2x2f(GlShader& glShader, std::string name, glm::mat2 data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniformMatrix2fv(glShader.uniformNameToLocation.at(name), 1, GL_FALSE, &data[0][0]);
}

static void SetUniformVec4f(GlShader& glShader, std::string name, glm::vec4 data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniform4f(glShader.uniformNameToLocation.at(name), data[0], data[1], data[2], data[3]);
}

static void SetUniformVec4fv(GlShader& glShader, std::string name, std::vector<glm::vec4> data, GLsizei size)
{
    if (!glShader.shader) { return; }
    if (data.empty()) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniform4fv(glShader.uniformNameToLocation.at(name), size, &data[0].x);
}

static void SetUniformVec3f(GlShader& glShader, std::string name, glm::vec3 data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniform3f(glShader.uniformNameToLocation.at(name), data[0], data[1], data[2]);
}

static void SetUniformVec3fv(GlShader& glShader, std::string name, std::vector<glm::vec3> data, GLsizei size)
{
    if (!glShader.shader) { return; }
    if (data.empty()) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniform3fv(glShader.uniformNameToLocation.at(name), size, &data[0].x);
}

static void SetUniformVec2f(GlShader& glShader, std::string name, glm::vec2 data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniform2f(glShader.uniformNameToLocation.at(name), data[0], data[1]);
}

static void SetUniformVec2fv(GlShader& glShader, std::string name, std::vector<glm::vec2> data, GLsizei size)
{
    if (!glShader.shader) { return; }
    if (data.empty()) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniform2fv(glShader.uniformNameToLocation.at(name), size, &data[0].x);
}

static void SetUniform1f(GlShader& glShader, std::string name, GLfloat data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniform1f(glShader.uniformNameToLocation.at(name), data);
}

static void SetUniform1fv(GlShader& glShader, std::string name, std::vector<GLfloat> data, GLsizei size)
{
    if (!glShader.shader) { return; }
    if (data.empty()) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniform1fv(glShader.uniformNameToLocation.at(name), size, &data[0]);
}

static void SetUniform1i(GlShader& glShader, std::string name, GLuint data)
{
    if (!glShader.shader) { return; }
    if (glShader.uniformNameToLocation.find(name) == glShader.uniformNameToLocation.end()) {
        GLint location = glGetUniformLocation(glShader.shader, name.c_str());
        if (location < 0) { return; }
        glShader.uniformNameToLocation[name] = location;
    }
    glUniform1i(glShader.uniformNameToLocation.at(name), data);
}
 
}
