#pragma once

#include <GL/glew.h>

#include "CrrtErrors.hpp"

namespace CRRT::GL {

/*
    OpenGL Texture Data Containers
*/

static constexpr GLuint EMPTY_GL_TEXTURE = 0;

struct GlTexture2D {
    GLuint  texture = 0;
    GLsizei width   = 0;
    GLsizei height  = 0;
};

struct GlTextureDataFormat {
    enum Format {
        RGBA8
        //...
    };
};

struct GlTexture2DConfig
{
    GlTextureDataFormat::Format format = GlTextureDataFormat::RGBA8;
    size_t width    = 0;
    size_t height   = 0;
    size_t channels = 4;

    bool   genMipmap    = false;
    GLenum minFiltering = GL_LINEAR;
    GLenum maxFiltering = GL_LINEAR;
    GLenum wrapS        = GL_REPEAT;
    GLenum wrapT        = GL_REPEAT;
    GLenum compareMode  = GL_NONE;
    GLenum compareFunc  = GL_LEQUAL;
};

/*
    Binding Texture as OpenGL Active Texture
*/

void BindTexture(GlTexture2D glTexture, GLuint glTextureUnit);

/*
    Loading and Releasing Textures to Device
*/

[[nodiscard]] Errors CreateEmptyTexture(GlTexture2D& glTexture, const GlTexture2DConfig& glConfig);
void ReleaseTexture(GlTexture2D& glTexture);

}
