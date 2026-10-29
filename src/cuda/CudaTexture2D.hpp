#pragma once

#include <cuda_runtime.h>

#include "CudaContext.hpp"
#include "CrrtErrors.hpp"
#include "../openGL/GlTexture2D.hpp"

/*
    Container to store all Cuda and OpenGL Texture Interop Resources
*/

namespace CRRT::CUDA {

struct CudaTexture2D
{
    cudaGraphicsResource* textureResource = nullptr;
    cudaSurfaceObject_t   surface = 0;
    bool                  isMemMapped = false;
};

/*
    Registering OpenGL textures and memory mapping to Cuda
*/

[[nodiscard]] Errors RegisterTextureInterop(const CudaContext& cudaContext, 
    CudaTexture2D& cudaTexture, const GL::GlTexture2D& glTexture);
[[nodiscard]] Errors UnregisterTextureInterop(const CudaContext& cudaContext, CudaTexture2D& cudaTexture);

[[nodiscard]] Errors MemMapTexture(const CudaContext& cudaContext,  CudaTexture2D& cudaTexture);
[[nodiscard]] Errors UnMemMapTexture(const CudaContext& context,CudaTexture2D& texture);

} 
