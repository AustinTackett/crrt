#pragma once

#include "../include/Crrt.hpp"

#include "CudaTexture2D.hpp"
#include "GlShader.hpp"
#include "GlVao.hpp"

namespace CRRT {

struct Frame
{
    CUDA::CudaContext   cudaContext;
    CUDA::CudaTexture2D cudaTexture;
    GL::GlVao           glVao;
    GL::GlTexture2D     glTexture;
    GL::GlShader        glShader;
};

/*
    Stores all the data of the pixels on screen, can begin draw pipeline to draw it to the screen, and 
    has functionality to open and close access of the pixel data to cuda.
*/

/*

From: include/Crrt.hpp

struct FrameView
{
    cudaSurfaceObject_t surface = 0;
    int width = 0;
    int height = 0;
};

*/

struct FrameView;
FrameView GetFrameView(const Frame& frame);

[[nodiscard]] inline Errors ReleaseFrame(Frame& frame);

[[nodiscard]] inline Errors CreateFrame(Frame& frame, int width, int height)
{
    Errors errors;
    if (frame.glVao.vao || frame.glTexture.texture || frame.glShader.shader ||
        frame.cudaTexture.textureResource || frame.cudaTexture.isMemMapped || 
        frame.cudaTexture.surface) 
    {
        errors |= ErrorCode::frameAlreadyCreated;
    }
    if (width <= 0 || height <= 0) {
        errors |= ErrorCode::glTextureInvalidDimensions;
    }
    if (errors) { return errors; }
    
    GL::GlTexture2DConfig textureConfig;
    textureConfig.width = width;
    textureConfig.height = height;

    errors = GL::CreateVao(frame.glVao);
    errors |= GL::LoadShader(frame.glShader);
    errors |= GL::CreateEmptyTexture(frame.glTexture, textureConfig);
    errors |= CUDA::FetchCudaContext(frame.cudaContext);
    if (errors) {
        return errors | ReleaseFrame(frame);
    }

    errors |= CUDA::RegisterTextureInterop(
        frame.cudaContext,
        frame.cudaTexture,
        frame.glTexture);
    if (errors) {
        return errors | ReleaseFrame(frame);
    }

    return errors;
}

[[nodiscard]] inline Errors ReleaseFrame(Frame& frame)
{
    Errors errors;
    
    errors = CUDA::UnMemMapTexture(frame.cudaContext, frame.cudaTexture);
    if (frame.cudaTexture.textureResource) {
        errors |= CUDA::UnregisterTextureInterop(frame.cudaContext, frame.cudaTexture);
    }
    if (!frame.cudaTexture.textureResource && !frame.cudaTexture.isMemMapped && !frame.cudaTexture.surface) {
        GL::ReleaseTexture(frame.glTexture);
        frame.cudaContext = {};
    }
    GL::ReleaseShader(frame.glShader);
    GL::ReleaseVao(frame.glVao); 

    return errors;
}

inline void Draw(Frame& frame)
{
    if (frame.cudaTexture.isMemMapped || frame.cudaTexture.surface) { return; }

    GL::BindShader(frame.glShader);
    GL::BindTexture(frame.glTexture, 0);
    GL::SetUniform1i(frame.glShader, "frameTexture", 0);
    GL::DrawVao(frame.glVao);
}

[[nodiscard]] inline Errors BeginCudaAccess(Frame& frame)
{
    return CUDA::MemMapTexture(frame.cudaContext, frame.cudaTexture);
}

[[nodiscard]] inline Errors EndCudaAccess(Frame& frame)
{
    return CUDA::UnMemMapTexture(frame.cudaContext, frame.cudaTexture);
}


};


