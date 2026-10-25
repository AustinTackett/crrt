#pragma once

#include "GlError.hpp"
#include <GL/glew.h>

namespace CRRT { namespace GL {

static constexpr GLuint EMPTY_VAO = 0;

struct GlVao
{
    GLuint vao = 0;
};

void BindVao(GlVao& glVao)
{
    glBindVertexArray(glVao.vao);
}

// Uses the currently bound shader, textures, framebuffer, and viewport.
void DrawVao(const GlVao& glVao, GLenum mode = GL_TRIANGLES, GLint first = 0, GLsizei count = 3)
{
    if (!glVao.vao) return;

    glBindVertexArray(glVao.vao);
    glDrawArrays(mode, first, count);
}

[[nodiscard]] GL::GlError CreateVao(GlVao& glVao)
{
    glGenVertexArrays(1, &glVao.vao);
    if (glVao.vao == EMPTY_VAO) { return GlError::vaoCreationFailed; }
    return GlError::glSuccess;
}

void ReleaseVao(GlVao& glVao)
{
    glDeleteVertexArrays(1, &glVao.vao);
    glVao.vao = 0;
}

}};
