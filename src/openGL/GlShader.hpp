#pragma once


#include <string>
#include <cstdio>
#include <unordered_map>

#include <GL/glew.h>

#include "CrrtErrors.hpp"

namespace CRRT { namespace GL {

/*
    Simple Shaders
    -> no need to add potentially problematic glsl file parsing if its literally only a few lines fixed function glsl
*/

inline constexpr GLchar vertexShaderString[] = R"glsl(
    #version 330 core

    out vec2 uv;

    // Full Screen Triangle
    const vec2 positions[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );

    void main()
    {
        vec2 position = positions[gl_VertexID];

        uv = position * 0.5 + 0.5;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";

inline constexpr GLchar fragmentShaderString[] = R"glsl(
    #version 330 core

    in vec2 uv;
    out vec4 color;

    uniform sampler2D frameTexture;

    void main()
    {
        color = texture(frameTexture, uv);
    }
)glsl";

/*
    OpenGL Shader Data Containers
*/

static constexpr GLuint EMPTY_GL_SHADER = 0;

struct GlShader
{
    GLuint shader = 0;
    std::unordered_map<std::string, GLint> uniformNameToLocation;
};

/*
    Loading and Releasing Shaders to Device
*/

inline void BindShader(const GlShader& glShader)
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

[[nodiscard]] inline Errors LoadShader(GlShader& shader)
{
    Errors errors;
    if (shader.shader) {
        errors |= ErrorCode::glShaderAlreadyCreated;
        return errors;
    }

    GLuint newVertexShader   = glCreateShader(GL_VERTEX_SHADER);
    GLuint newFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    bool shaderCreationSuccess = newVertexShader && newFragmentShader;
    if (!shaderCreationSuccess) {
        if (newVertexShader)   { glDeleteShader(newVertexShader);   }
        if (newFragmentShader) { glDeleteShader(newFragmentShader); }
        errors |= ErrorCode::glShaderCreationFailed;
        return errors;
    }

    const GLchar* vshaderCharArray = vertexShaderString;
    const GLchar* fshaderCharArray = fragmentShaderString;
    glShaderSource(newVertexShader,   1, &vshaderCharArray, NULL);
    glShaderSource(newFragmentShader, 1, &fshaderCharArray, NULL);

    GLint vshaderCompileErr = 0;
    glCompileShader(newVertexShader);
    glGetShaderiv(newVertexShader, GL_COMPILE_STATUS, &vshaderCompileErr);
    if (!vshaderCompileErr)
    {
        errors |= ErrorCode::glShaderVertexCompilationFailed;
    }

    GLint fshaderCompileErr = 0;
    glCompileShader(newFragmentShader);
    glGetShaderiv(newFragmentShader, GL_COMPILE_STATUS, &fshaderCompileErr);
    if (!fshaderCompileErr)
    {
        errors |= ErrorCode::glShaderFragmentCompilationFailed;
    }

    if (errors) {
        glDeleteShader(newVertexShader);
        glDeleteShader(newFragmentShader);
        return errors;
    }

    GLint linkProgramErr = 0;
    GLuint newShaderProgram = glCreateProgram();
    if (!newShaderProgram)
    {
        glDeleteShader(newVertexShader);
        glDeleteShader(newFragmentShader);
        errors |= ErrorCode::glShaderProgramCreationFailed;
        return errors;
    }

    glAttachShader(newShaderProgram, newVertexShader);
    glAttachShader(newShaderProgram, newFragmentShader);
    glLinkProgram(newShaderProgram);
    glGetProgramiv(newShaderProgram, GL_LINK_STATUS, &linkProgramErr);
    if (!linkProgramErr)
    {
        glDeleteShader(newVertexShader);
        glDeleteShader(newFragmentShader);
        glDeleteProgram(newShaderProgram);
        errors |= ErrorCode::glShaderProgramLinkFailed;
        return errors;
    }

    glDetachShader(newShaderProgram, newVertexShader);
    glDetachShader(newShaderProgram, newFragmentShader);
    glDeleteShader(newVertexShader);
    glDeleteShader(newFragmentShader);

    shader.shader = newShaderProgram;
    shader.uniformNameToLocation.clear();
    return errors;
}

inline void ReleaseShader(GlShader& glShader)
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

inline void SetUniform1i(GlShader& glShader, std::string name, GLuint data)
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



