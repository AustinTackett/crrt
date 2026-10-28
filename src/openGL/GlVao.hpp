#pragma once


#include <GL/glew.h>

#include "CrrtErrors.hpp"

namespace CRRT { namespace GL {

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


inline void BindVao(GlVao& glVao)
{
    glBindVertexArray(glVao.vao);
}

// Uses the currently bound shader, textures, framebuffer, and viewport.
inline void DrawVao(const GlVao& glVao, GLenum mode = GL_TRIANGLES, GLint first = 0, GLsizei count = 3)
{
    if (!glVao.vao) return;

    glBindVertexArray(glVao.vao);
    glDrawArrays(mode, first, count);
}

[[nodiscard]] inline Errors CreateVao(GlVao& glVao)
{
    Errors errors;
    if (glVao.vao) {
        errors |= ErrorCode::glVaoAlreadyCreated;
        return errors;
    }
    glGenVertexArrays(1, &glVao.vao);
    if (glVao.vao == EMPTY_VAO) {
        errors |= ErrorCode::glVaoCreationFailed;
    }
    return errors;
}

inline void ReleaseVao(GlVao& glVao)
{
    if (!glVao.vao) { return; }
    
    glDeleteVertexArrays(1, &glVao.vao);
    glVao.vao = 0;
}

}};
