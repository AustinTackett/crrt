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

namespace CRRT { namespace CUDA {

struct CudaTexture2D
{
    cudaGraphicsResource* textureResource = nullptr;
    cudaResourceDesc      textureDesc{};
    cudaSurfaceObject_t   surface = 0;
    bool                  isMemMapped = false;
};


// Forward Declarations
[[nodiscard]] inline Errors UnMemMapTexture(const CudaContext& cudaContext, CudaTexture2D& cudaTexture);

/*
    Registering OpenGL textures and memory mapping to Cuda
*/

[[nodiscard]] inline Errors RegisterTextureInterop(
    const CudaContext& cudaContext, 
    CudaTexture2D& cudaTexture, 
    const GL::GlTexture2D& glTexture)
{
    Errors errors;

    if (cudaTexture.textureResource) { 
        errors |= ErrorCode::cudaTextureAlreadyRegistered; 
    }
    if (!glTexture.texture) { 
        errors |= ErrorCode::cudaInvalidGlTexture;
    }
    if (errors) { 
        return errors; 
    }

    if (cudaSetDevice(cudaContext.cudaDevice)) {
        errors |= ErrorCode::cudaSetDeviceFailed;
        return errors;
    }
    if (cudaGraphicsGLRegisterImage(
        &cudaTexture.textureResource,
        glTexture.texture,
        GL_TEXTURE_2D,
        cudaGraphicsRegisterFlagsSurfaceLoadStore))
    {
        errors |= ErrorCode::cudaTextureRegistrationFailed;
        return errors;
    }

    return errors;
}

[[nodiscard]] inline Errors UnregisterTextureInterop(const CudaContext& cudaContext, CudaTexture2D& cudaTexture)
{
    Errors errors;

    if (!cudaTexture.textureResource) {
        errors |= ErrorCode::cudaTextureNotRegistered;
    }
    if (cudaTexture.isMemMapped || cudaTexture.surface) {
        errors |= ErrorCode::cudaTextureAlreadyMapped;
    }
    if (errors) {
        return errors;
    }

    if (cudaSetDevice(cudaContext.cudaDevice)) {
        errors |= ErrorCode::cudaSetDeviceFailed;
        return errors;
    }
    if (cudaGraphicsUnregisterResource(cudaTexture.textureResource)) {
        errors |= ErrorCode::cudaTextureUnregistrationFailed;
        return errors;
    }

    cudaTexture = {};
    return errors;
}

[[nodiscard]] inline Errors MemMapTexture(const CudaContext& cudaContext,
    CudaTexture2D& cudaTexture)
{
    Errors errors;
    if (!cudaTexture.textureResource) { 
        errors |= ErrorCode::cudaTextureNotRegistered; 
    }
    if (cudaTexture.isMemMapped || cudaTexture.surface) { 
        errors |= ErrorCode::cudaTextureAlreadyMapped;
    }
    if (errors) { 
        return errors; 
    }

    if (cudaSetDevice(cudaContext.cudaDevice)) {
        errors |= ErrorCode::cudaSetDeviceFailed;
        return errors;
    }
    if (cudaGraphicsMapResources(1, &cudaTexture.textureResource, 0)) {
        errors |= ErrorCode::cudaTextureMapFailed;
        return errors;
    }
    cudaTexture.isMemMapped = true;

    // We need a new surface object every frame according to Nividia
    cudaTexture.textureDesc = {};
    cudaResourceDesc textureDesc{};
    textureDesc.resType = cudaResourceTypeArray;
    cudaSurfaceObject_t surface = 0;
    if (cudaGraphicsSubResourceGetMappedArray(
        &textureDesc.res.array.array,
        cudaTexture.textureResource,
        0,
        0)) 
    {
        errors |= ErrorCode::cudaMappedArrayQueryFailed;
    } else if (cudaCreateSurfaceObject(
        &surface,
        &textureDesc)) 
    {
        errors |= ErrorCode::cudaSurfaceCreationFailed;
    } else {
        cudaTexture.textureDesc = textureDesc;
        cudaTexture.surface = surface;
    }

    if (errors) {
        errors |= UnMemMapTexture(cudaContext, cudaTexture);
    }
    return errors;
}

[[nodiscard]] inline Errors UnMemMapTexture(const CudaContext& cudaContext,
    CudaTexture2D& cudaTexture)
{
    Errors errors;
    if (!cudaTexture.isMemMapped && !cudaTexture.surface) {
        cudaTexture.textureDesc = {};
        return errors;
    }

    if (cudaSetDevice(cudaContext.cudaDevice)) {
        errors |= ErrorCode::cudaSetDeviceFailed;
        return errors;
    }
    if (cudaDeviceSynchronize()) {
        errors |= ErrorCode::cudaDeviceSynchronizationFailed;
    }

    if (cudaTexture.surface) {
        if (cudaDestroySurfaceObject(cudaTexture.surface)) {
            errors |= ErrorCode::cudaSurfaceDestructionFailed;
        } else {
            cudaTexture.surface = 0;
        }
    }

    if (cudaTexture.isMemMapped && !cudaTexture.surface) {
        if (cudaGraphicsUnmapResources(
            1,
            &cudaTexture.textureResource,
            0)) 
        {
            errors |= ErrorCode::cudaTextureUnmapFailed;
        } else {
            cudaTexture.isMemMapped = false;
        }
    }

    if (!cudaTexture.isMemMapped && !cudaTexture.surface) {
        cudaTexture.textureDesc = {};
    }
    return errors;
}

}};
