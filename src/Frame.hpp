#pragma once

#include <string>
#include <string_view>

#include "CudaError.hpp"
#include "CudaTexture2D.hpp"
#include "GlShader.hpp"
#include "openGL/GlError.hpp"
#include "openGL/GlVao.hpp"

namespace CRRT {

constexpr std::string_view VERTEX_SHADER_PATH   = "src/openGL/GlShaders/GIRenderTexture2D.vert";
constexpr std::string_view FRAGMENT_SHADER_PATH = "src/openGL/GlShaders/GlRenderTexture2D.frag";

/*
    Tracking if Frame Operation Returned An Error
*/

struct FrameError {
    bool valid = true;

    CUDA::CudaError cuda = cudaSuccess;
    GL::GlError     gl   = GL::GlError::glSuccess; 
};

bool hasFrameError(const FrameError& error)
{
    return !error.valid;
}

/*
    Frame contains all data from Cuda and OpenGL that is need to draw and write to the screen
*/

struct Frame
{
    CUDA::CudaContext   cudaContext;
    CUDA::CudaTexture2D cudaTexture;

    GL::GlVao           glVao;
    GL::GlTexture2D     glTexture;
    GL::GlShader        glShader;
};

/*
    Creating/Destroying Frame Object
*/

[[nodiscard]] FrameError CreateFrame(Frame& frame, int width, int height)
{    
    GL::GlTexture2DConfig config;
    config.format       = GL::GlTextureDataFormat::RGBA8;
    config.width        = width;
    config.height       = height;

    GL::GlShaderConfig shaderConfig = { 
        std::string(VERTEX_SHADER_PATH), 
        std::string(FRAGMENT_SHADER_PATH) 
    };

    auto glError = GL::CreateVao(frame.glVao);
    if (glError != GL::GlError::glSuccess) return {false, cudaSuccess, glError};
    glError = GL::LoadShader(frame.glShader, shaderConfig);
    if (glError != GL::GlError::glSuccess) return {false, cudaSuccess, glError};
    glError = GL::CreateEmptyTexture(frame.glTexture, config);
    if (glError != GL::GlError::glSuccess) return {false, cudaSuccess, glError};

    auto cudaError = CUDA::FetchCudaContext(frame.cudaContext);
    if (cudaError != cudaSuccess) return {false, cudaError};
    cudaError = CUDA::RegisterTextureInterop(frame.cudaContext, frame.cudaTexture, frame.glTexture);
    if (cudaError != cudaSuccess) return {false, cudaError};

    return FrameError{};
}

[[nodiscard]] FrameError ReleaseFrame(Frame& frame)
{
    auto error = CUDA::UnMemMapTexture(frame.cudaContext, frame.cudaTexture);
    if (error != cudaSuccess) return {false, error};
    error = CUDA::UnregisterTextureInterop(frame.cudaContext, frame.cudaTexture);
    
    if (error != cudaSuccess) return {false, error};
    GL::ReleaseTexture(frame.glTexture);
    GL::ReleaseShader(frame.glShader);
    
    if (frame.glVao.vao) GL::ReleaseVao(frame.glVao);

    return FrameError{};;
}

void Draw(Frame& frame)
{
    if (frame.cudaTexture.isMemMapped) return;

    GL::BindShader(frame.glShader);
    GL::BindTexture(frame.glTexture, GL_TEXTURE0);
    GL::SetUniform1i(frame.glShader, "frameTexture", 0);
    GL::DrawVao(frame.glVao);
}

// Give control of OpenGL resources to Cuda
[[nodiscard]] FrameError BeginCudaAccess(Frame& frame)
{
    if (!frame.cudaTexture.textureResource) {
        return {false, cudaErrorInvalidResourceHandle};
    }
    if (frame.cudaTexture.isMemMapped) {
        return {false, cudaErrorInvalidResourceHandle};
    }

    const auto error = CUDA::MemMapTexture(frame.cudaContext, frame.cudaTexture);
    return {error == cudaSuccess, error};
}

// Return control to OpenGL
[[nodiscard]] FrameError EndCudaAccess(Frame& frame)
{
    const auto error = CUDA::UnMemMapTexture(frame.cudaContext, frame.cudaTexture);
    return {error == cudaSuccess, error};
}

// IMPORTANT: Make sure to use BeginCudaAccess() before attempting to read or write to frame in cuda kernel 
__device__ void   Write(Frame& frame, int x, int y, uchar4 color);
__device__ uchar4 Read (Frame& frame, int x, int y);

}
