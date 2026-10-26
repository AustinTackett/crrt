#pragma once

#include <cstdio>
#include <string> 

#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <driver_types.h>

namespace CRRT { 

/*
    OpenGL Related Errors
    -> OpenGL has its own error retrieval system, but its primarily for debugging
*/

namespace GL {

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

}
    
namespace CUDA {

/*
    Cuda Related Errors
*/

// No need to make your own error enum and toString() because cuda already made it
using CudaError = cudaError_t;
inline std::string CudaGetErrorString(CudaError error) 
{
    return std::string(cudaGetErrorString(error));
}

}

/*
    Errors Generated From the Window
*/

enum class WindowErrorCode
{
    windowsSuccess,
    windowsAlreadyCreated,
    windowsInvalidWindow,
    windowsGlfwInitializationFailed,
    windowsCreationFailed,
    windowsContextActivationFailed,
    windowsGlewInitializationFailed,
    windowsFrameOperationFailed,
    windowsFrameCleanupFailed,
    windowDrawCallbackException,
};

inline std::string GetWindowErrorString(WindowErrorCode error)
{
    switch (error) {
        case WindowErrorCode::windowsSuccess:                  return "Success";
        case WindowErrorCode::windowsAlreadyCreated:           return "Window already created";
        case WindowErrorCode::windowsInvalidWindow:            return "Invalid window";
        case WindowErrorCode::windowsGlfwInitializationFailed: return "GLFW initialization failed";
        case WindowErrorCode::windowsCreationFailed:           return "Window creation failed";
        case WindowErrorCode::windowsContextActivationFailed:  return "Window context activation failed";
        case WindowErrorCode::windowsGlewInitializationFailed: return "GLEW initialization failed";
        case WindowErrorCode::windowsFrameOperationFailed:     return "Window frame operation failed";
        case WindowErrorCode::windowsFrameCleanupFailed:       return "Window frame cleanup failed";
        case WindowErrorCode::windowDrawCallbackException:     return "Window draw callback threw an exception";
        default:                                               return "Unknown window error";
    }
}

};
