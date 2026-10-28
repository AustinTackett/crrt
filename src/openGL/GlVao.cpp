#include "GlVao.hpp"

namespace CRRT::GL {

void BindVao(GlVao& glVao)
{
    glBindVertexArray(glVao.vao);
}

void DrawVao(const GlVao& glVao, GLenum mode, GLint first, GLsizei count)
{
    if (!glVao.vao) return;

    glBindVertexArray(glVao.vao);
    glDrawArrays(mode, first, count);
}

[[nodiscard]] Errors CreateVao(GlVao& glVao)
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

void ReleaseVao(GlVao& glVao)
{
    if (!glVao.vao) { return; }
    
    glDeleteVertexArrays(1, &glVao.vao);
    glVao.vao = 0;
}

}
