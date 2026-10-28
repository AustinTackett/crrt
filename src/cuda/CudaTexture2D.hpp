#pragma once

#include <GL/glew.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include "CudaContext.hpp"
#include "CrrtErrors.hpp"
#include "GlTexture2D.hpp"

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

// Forward Declarations
[[nodiscard]] Errors UnMemMapTexture(const CudaContext& cudaContext, CudaTexture2D& cudaTexture);

/*
    Registering OpenGL textures and memory mapping to Cuda
*/

[[nodiscard]] Errors RegisterTextureInterop(
    const CudaContext& cudaContext, 
    CudaTexture2D& cudaTexture, 
    const GL::GlTexture2D& glTexture);

[[nodiscard]] Errors UnregisterTextureInterop(const CudaContext& cudaContext, CudaTexture2D& cudaTexture);

[[nodiscard]] Errors MemMapTexture(const CudaContext& cudaContext,
    CudaTexture2D& cudaTexture);

[[nodiscard]] Errors UnMemMapTexture(const CudaContext& context,CudaTexture2D& texture);

} 
