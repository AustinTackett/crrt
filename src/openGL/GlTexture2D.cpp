#include "GlTexture2D.hpp"

namespace CRRT::GL {

void BindTexture(GlTexture2D glTexture, GLuint glTextureUnit)
{
    glActiveTexture(GL_TEXTURE0 + glTextureUnit);
    glBindTexture(GL_TEXTURE_2D, glTexture.texture);
}

[[nodiscard]] Errors CreateEmptyTexture(GlTexture2D& glTexture, const GlTexture2DConfig& glConfig)
{
    Errors errors;
    if (glTexture.texture) {
        errors |= ErrorCode::glTextureAlreadyCreated;
    }

    // Check for valid parameters
    auto isInvalidDimensions = [](size_t width, size_t height) -> bool {
        return width == 0 || height == 0 ||
               width  > static_cast<size_t>((std::numeric_limits<GLsizei>::max)()) ||
               height > static_cast<size_t>((std::numeric_limits<GLsizei>::max)());
    };
    if (isInvalidDimensions(glConfig.width, glConfig.height)) {
        errors |= ErrorCode::glTextureInvalidDimensions;
    }
    if (errors) {
        return errors;
    }

    // Create empty texture
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
        // .. Where other texture formats would be loaded
        default:
        {
            errors |= ErrorCode::glTextureUnsupportedFormat;
            glDeleteTextures(1, &newTexture);
            return errors;
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
    glTexture.width   = static_cast<GLsizei>(glConfig.width);
    glTexture.height  = static_cast<GLsizei>(glConfig.height);
    return errors;
}

void ReleaseTexture(GlTexture2D& glTexture)
{
    if (!glTexture.texture) return;

    glDeleteTextures(1, &glTexture.texture);
    glTexture.texture = EMPTY_GL_TEXTURE;
    glTexture.width   = 0;
    glTexture.height  = 0;
}

}
