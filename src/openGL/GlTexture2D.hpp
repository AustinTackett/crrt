#pragma once


#include <limits>

#include <GL/glew.h>

#include "CrrtErrors.hpp"

namespace CRRT { namespace GL {

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

inline void BindTexture(GlTexture2D glTexture, GLuint glTextureUnit)
{
    glActiveTexture(GL_TEXTURE0 + glTextureUnit);
    glBindTexture(GL_TEXTURE_2D, glTexture.texture);
}

/*
    Loading and Releasing Textures to Device
*/

[[nodiscard]] inline Errors CreateEmptyTexture(GlTexture2D& glTexture, const GlTexture2DConfig& glConfig)
{
    Errors errors;
    if (glTexture.texture) {
        errors |= ErrorCode::glTextureAlreadyCreated;
    }

    auto isInvalidDimensions = [](size_t width, size_t height) -> bool {
        return width == 0 || height == 0 ||
               width  > static_cast<size_t>((std::numeric_limits<GLsizei>::max)()) ||
               height > static_cast<size_t>((std::numeric_limits<GLsizei>::max)());
    };
    if (isInvalidDimensions(glConfig.width, glConfig.height)) {
        errors |= ErrorCode::glTextureInvalidDimensions;
    }
    if (glConfig.format != GlTextureDataFormat::RGBA8) {
        errors |= ErrorCode::glTextureUnsupportedFormat;
    }
    if (errors) {
        return errors;
    }

    GLuint newTexture = EMPTY_GL_TEXTURE;
    glGenTextures(1, &newTexture);
    if (newTexture == EMPTY_GL_TEXTURE) {
        errors |= ErrorCode::glTextureCreationFailed;
        return errors;
    }

    glBindTexture(GL_TEXTURE_2D, newTexture);

    // Load As Correct format (only RGBA8 supported at this moment)
    switch(glConfig.format){
        case GL::GlTextureDataFormat::RGBA8: {
            glTexImage2D(GL_TEXTURE_2D,
                0,
                GL_RGBA8,
                glConfig.width,
                glConfig.height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                nullptr);
            break;
        }
        default:
        {
            break;
        }
    }
    if (glGetError() != GL_NO_ERROR) {
        errors |= ErrorCode::glTextureCreationFailed;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glConfig.minFiltering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glConfig.maxFiltering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glConfig.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glConfig.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, glConfig.compareMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, glConfig.compareFunc);
    if (glConfig.genMipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    if (glGetError() != GL_NO_ERROR) {
        errors |= ErrorCode::glTextureConfigurationFailed;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    if (errors) {
        glDeleteTextures(1, &newTexture);
        return errors;
    }

    glTexture.texture = newTexture;
    glTexture.width  = static_cast<GLsizei>(glConfig.width);
    glTexture.height = static_cast<GLsizei>(glConfig.height);
    return errors;
}

inline void ReleaseTexture(GlTexture2D& glTexture)
{
    if (!glTexture.texture) return;

    glDeleteTextures(1, &glTexture.texture);
    glTexture.texture = EMPTY_GL_TEXTURE;
    glTexture.width   = 0;
    glTexture.height  = 0;
}

}};
