#pragma once

#include <string>

/*
    OpenGL Related Errors
    -> OpenGL has its own error retrieval system, but its primarily for debugging
*/

namespace CRRT { namespace GL {

enum class GlError
{
    glSuccess = 0,
    textureAlreadyCreated,
    textureInvalidDimensions,
    textureUnsupportedFormat,
    textureCreationFailed,
    textureConfigurationFailed,
    shaderVertexFileReadFailed,
    shaderFragmentFileReadFailed,
    shaderCreationFailed,
    shaderVertexCompilationFailed,
    shaderFragmentCompilationFailed,
    shaderProgramCreationFailed,
    shaderProgramLinkFailed,
    shaderAlreadyCreated,
    vaoCreationFailed,
};

inline std::string glGetErrorString(GlError error)
{
    switch (error) {
        case GlError::glSuccess:                       return "Success";
        case GlError::textureAlreadyCreated:           return "Texture already created";
        case GlError::textureInvalidDimensions:        return "Invalid texture dimensions";
        case GlError::textureUnsupportedFormat:        return "Unsupported texture format";
        case GlError::textureCreationFailed:           return "Texture creation failed";
        case GlError::textureConfigurationFailed:      return "Texture configuration failed";
        case GlError::shaderVertexFileReadFailed:      return "Vertex shader file read failed";
        case GlError::shaderFragmentFileReadFailed:    return "Fragment shader file read failed";
        case GlError::shaderCreationFailed:            return "Shader creation failed";
        case GlError::shaderVertexCompilationFailed:   return "Vertex shader compilation failed";
        case GlError::shaderFragmentCompilationFailed: return "Fragment shader compilation failed";
        case GlError::shaderProgramCreationFailed:     return "Shader program creation failed";
        case GlError::shaderProgramLinkFailed:         return "Shader program linking failed";
        case GlError::shaderAlreadyCreated:            return "Shader already created";
        case GlError::vaoCreationFailed:               return "VAO creation failed";
        default:                                       return "Unknown OpenGL error";
    }
}

}};
