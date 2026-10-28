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

    if (cudaTexture.textureResource || cudaTexture.isMemMapped || cudaTexture.surface)
    {
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

    cudaGraphicsResource_t resource = nullptr;
    if (cudaGraphicsGLRegisterImage(
        &resource,
        glTexture.texture,
        GL_TEXTURE_2D,
        cudaGraphicsRegisterFlagsSurfaceLoadStore))
    {
        errors |= ErrorCode::cudaTextureRegistrationFailed;
        return errors;
    }

    cudaTexture.textureResource = resource;
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

    // Map OpenGL Texture Resource to Cuda
    if (cudaSetDevice(cudaContext.cudaDevice)) {
        errors |= ErrorCode::cudaSetDeviceFailed;
        return errors;
    }
    if (cudaGraphicsMapResources(1, &cudaTexture.textureResource, 0)) {
        errors |= ErrorCode::cudaTextureMapFailed;
        return errors;
    }

    // Give Surface Object to let Cuda Access Texture 
    cudaTexture.isMemMapped = true;
    cudaArray_t array = nullptr;
    if (cudaGraphicsSubResourceGetMappedArray(
        &array,
        cudaTexture.textureResource,
        0,
        0) != cudaSuccess)
    {
        errors |= ErrorCode::cudaMappedArrayQueryFailed;
        return errors | UnMemMapTexture(cudaContext, cudaTexture);
    }
    cudaSurfaceObject_t surface = 0;
    cudaResourceDesc description{};
    description.resType = cudaResourceTypeArray;
    description.res.array.array = array;
    if (cudaCreateSurfaceObject(
        &surface,
        &description)) 
    {
        errors |= ErrorCode::cudaSurfaceCreationFailed;
        return errors | UnMemMapTexture(cudaContext, cudaTexture);
    }

    cudaTexture.surface = surface;
    return errors;
}

[[nodiscard]] inline Errors UnMemMapTexture(const CudaContext& context,CudaTexture2D& texture)
{
    Errors errors;
    if (!texture.isMemMapped && !texture.surface)
        return errors;
    if (cudaSetDevice(context.cudaDevice) != cudaSuccess) {
        errors |= ErrorCode::cudaSetDeviceFailed;
        return errors;
    }

    // Ensure all previously submitted CUDA work completed
    if (cudaDeviceSynchronize()) {
        errors |= ErrorCode::cudaDeviceSynchronizationFailed;
        return errors;
    }

    // Surface must be destroyed before unmapping
    if (texture.surface) {
        if (cudaDestroySurfaceObject(texture.surface)) {
            errors |= ErrorCode::cudaSurfaceDestructionFailed;
            return errors;
        }

        texture.surface = 0;
    }

    // Return the resource to OpenGL
    if (texture.isMemMapped) {
        if (cudaGraphicsUnmapResources(
            1,
            &texture.textureResource,
            0)) 
        {
            errors |= ErrorCode::cudaTextureUnmapFailed;
            return errors;
        }

        texture.isMemMapped = false;
    }

    return errors;
}

}};
