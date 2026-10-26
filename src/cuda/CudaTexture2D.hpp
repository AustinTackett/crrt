#pragma once

#include <GL/glew.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include "CudaContext.hpp"
#include "CudaMacros.hpp"
#include "openGL/GlTexture2D.hpp"
#include "ErrorCodes.hpp"

/*
    Container to store all Cuda and OpenGL Texture Interop Resources
*/

namespace CRRT { namespace CUDA {

struct CudaTexture2D
{
    cudaGraphicsResource* textureResource = nullptr;
    cudaResourceDesc      textureDesc{};
    cudaSurfaceObject_t   surface = 0;
    bool                  isMemMapped = false;
};

[[nodiscard]] CudaError RegisterTextureInterop(const CudaContext& cudaContext,
    CudaTexture2D& cudaTexture, const GL::GlTexture2D& glTexture)
{
    if (cudaTexture.textureResource) { return CudaError::cudaErrorInvalidResourceHandle; }
    if (!glTexture.texture)          { return CudaError::cudaErrorInvalidValue;          }

    CUDA_CHECK_OR_RETURN(cudaSetDevice(cudaContext.cudaDevice));
    CUDA_CHECK_OR_RETURN(cudaGraphicsGLRegisterImage(
        &cudaTexture.textureResource, glTexture.texture, GL_TEXTURE_2D,
        cudaGraphicsRegisterFlagsSurfaceLoadStore
    ));

    cudaTexture.textureDesc.resType = cudaResourceTypeArray;
    return cudaSuccess;
}

[[nodiscard]] CudaError UnregisterTextureInterop(const CudaContext& cudaContext,
    CudaTexture2D& cudaTexture)
{
    if (cudaTexture.isMemMapped || cudaTexture.surface) return cudaErrorInvalidResourceHandle;
    if (!cudaTexture.textureResource) return cudaSuccess;

    CUDA_CHECK_OR_RETURN(cudaSetDevice(cudaContext.cudaDevice));
    CUDA_CHECK_OR_RETURN(cudaGraphicsUnregisterResource(cudaTexture.textureResource));

    cudaTexture = {};
    return cudaSuccess;
}

[[nodiscard]] CudaError MemMapTexture(const CudaContext& cudaContext,
    CudaTexture2D& cudaTexture)
{
    if (!cudaTexture.textureResource) return cudaErrorInvalidResourceHandle;
    if (cudaTexture.isMemMapped || cudaTexture.surface) return cudaErrorInvalidResourceHandle;

    CUDA_CHECK_OR_RETURN(cudaSetDevice(cudaContext.cudaDevice));
    CUDA_CHECK_OR_RETURN(cudaGraphicsMapResources(1, &cudaTexture.textureResource, 0));
    cudaTexture.isMemMapped = true;
    cudaTexture.textureDesc.resType = cudaResourceTypeArray;

    CUDA_CHECK_OR_RETURN(cudaGraphicsSubResourceGetMappedArray(
        &cudaTexture.textureDesc.res.array.array, 
        cudaTexture.textureResource, 
        0, 
        0
    ));
    CUDA_CHECK_OR_RETURN(cudaCreateSurfaceObject(
        &cudaTexture.surface, 
        &cudaTexture.textureDesc
    ));

    return cudaSuccess;
}

[[nodiscard]] CudaError UnMemMapTexture(const CudaContext& cudaContext,
    CudaTexture2D& cudaTexture)
{
    if (!cudaTexture.isMemMapped && !cudaTexture.surface) return cudaSuccess;

    CUDA_CHECK_OR_RETURN(cudaSetDevice(cudaContext.cudaDevice));
    CUDA_CHECK_OR_RETURN(cudaDeviceSynchronize());

    if (cudaTexture.surface) {
        CUDA_CHECK_OR_RETURN(cudaDestroySurfaceObject(cudaTexture.surface));
        cudaTexture.surface = 0;
    }

    if (cudaTexture.isMemMapped) {
        CUDA_CHECK_OR_RETURN(cudaGraphicsUnmapResources(
            1, 
            &cudaTexture.textureResource, 
            0
        ));
        cudaTexture.isMemMapped = false;
    }

    cudaTexture.textureDesc = {};
    return cudaSuccess;
}

}};
