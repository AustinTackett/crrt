#pragma once

#include <GL/glew.h>

#include "CrrtErrors.hpp"

namespace CRRT::GL {

/*
    OpenGL Vertex Array
    -> Not alot here because we just hardcoded a full screen triangle in the vertex shader and
       don't pass in any other vertex data, but a vao is still required in the OpenGL pipeline
*/

static constexpr GLuint EMPTY_VAO = 0;

struct GlVao
{
    GLuint vao = 0;
};

void BindVao(GlVao& glVao);

[[nodiscard]] Errors CreateVao(GlVao& glVao);
void ReleaseVao(GlVao& glVao);

// Uses the currently bound shader, textures, framebuffer, and viewport.
void DrawVao(const GlVao& glVao, GLenum mode = GL_TRIANGLES, GLint first = 0, GLsizei count = 3);

}
