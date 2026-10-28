#pragma once

#include <string>
#include <cstdio>
#include <unordered_map>

#include <GL/glew.h>

#include "CrrtErrors.hpp"

namespace CRRT::GL {

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

void BindShader(const GlShader& glShader);

[[nodiscard]] Errors LoadShader(GlShader& shader);
void ReleaseShader(GlShader& glShader);

/*
    Setting Shader Uniforms
*/

void SetUniform1i(GlShader& glShader, std::string name, GLuint data);

}

