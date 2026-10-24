#pragma once

#include <stdexcept>

#include <GL/glew.h>

namespace GL {

/*
    OpenGL Texture Data Containers
*/

static constexpr GLuint EMPTY_GL_TEXTURE = 0;

struct GlTextureDataFormat {
    enum Format { 
        RGBA8
        //...
    };
};

struct GlTexture2DConfig
{
    GlTextureDataFormat::Format format;
    size_t width    = 0;
    size_t height   = 0;
    size_t channels = 4;

    bool   genMipmap = true;
    GLenum minFiltering = GL_LINEAR_MIPMAP_LINEAR;
    GLenum maxFiltering = GL_LINEAR;
    GLenum wrapS = GL_REPEAT;
    GLenum wrapT = GL_REPEAT;
    GLenum compareMode = GL_NONE;
    GLenum compareFunc = GL_LEQUAL; 
};

struct GlTexture2D {
    GLuint  texture = EMPTY_GL_TEXTURE;
    GLsizei width   = 0;
    GLsizei height  = 0;

    GlTextureDataFormat::Format format;
};

/*
    Binding Texture as OpenGL Active Texture
*/

static void BindTexture(GLuint glTexture, GLuint glTextureUnit)
{
    glActiveTexture(GL_TEXTURE0 + glTextureUnit);
    glBindTexture(GL_TEXTURE_2D, glTexture);
}

/*
    Loading and Releasing Textures to Device
*/

static GlTexture2D CreateEmptyTexture(GL::GlTexture2DConfig glConfig)
{
    GlTexture2D glTexture;

    glGenTextures(1, &glTexture.texture);
    if (glTexture.texture == EMPTY_GL_TEXTURE) {
        throw std::runtime_error("OpenGL failed to allocate new texture");
    }

    glBindTexture(GL_TEXTURE_2D, glTexture.texture);

    // Load As Correct format (only RGBA8 supported at this moment)
    switch(glConfig.format){
        case GL::GlTextureDataFormat::RGBA8: 
        {
            glTexImage2D(GL_TEXTURE_2D, 
                0, 
                GL_RGBA8, 
                glConfig.width, 
                glConfig.height, 
                0, 
                GL_RGBA8, 
                GL_UNSIGNED_BYTE, 
                nullptr);
            break;
        }
        default:
        {
            throw std::runtime_error("Unsupported texture data format tried to load in OpenGL");
            break;
        }
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

    glTexture.width = glConfig.width;
    glTexture.height = glConfig.height;

    glBindTexture(GL_TEXTURE_2D, 0);
    return glTexture;
}

static void ReleaseTexture(GlTexture2D& glTexture)
{
    if (!glTexture.texture) return;

    glDeleteTextures(1, &glTexture.texture);
    glTexture.texture = EMPTY_GL_TEXTURE;
    glTexture.width   = 0;
    glTexture.height  = 0;
}

};