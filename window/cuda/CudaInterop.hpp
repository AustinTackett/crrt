#pragma once

#include <stdexcept>

#include <GL/glew.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include "CudaErrors.hpp"
#include "CudaTexture2D.hpp"
#include "GlTexture2D.hpp"

namespace CUDA {

/*
    Container to store all Cuda and OpenGL Interop Resources
*/

struct CudaContext
{
    // Cuda Device Informatiaon
    unsigned int cudaDeviceCount = 0;
    int cudaDevice = -1;
};

static CudaContext FetchCudaContext()
{
    CudaContext context;

    CUDA_ASSERT(cudaGLGetDevices(
        &context.cudaDeviceCount, 
        &context.cudaDevice,
        1,
        cudaGLDeviceListAll
    ));

    if (context.cudaDeviceCount == 0) {
        throw std::runtime_error("No CUDA-compatible OpenGL device found");
    }
    CUDA_ASSERT(cudaSetDevice(context.cudaDevice));

    return context;
}

/*
    Functions Add OpenGL Resource To Cuda
*/

static CudaTexture2D RegisterTextureInterop(const CUDA::CudaContext& cudaContext, const GL::GlTexture2D& glTexture)
{
    // Register With Cuda For Reading and Writing
    CudaTexture2D cudaTexture;
    cudaTexture.textureDesc.resType = cudaResourceTypeArray;

    CUDA_ASSERT(cudaSetDevice(cudaContext.cudaDevice));
    CUDA_ASSERT(cudaGraphicsGLRegisterImage(
        &cudaTexture.textureResource,
        glTexture.texture,
        GL_TEXTURE_2D,
        cudaGraphicsRegisterFlagsSurfaceLoadStore
    ));

    return cudaTexture;
}

static void UnregisterTextureInterop(CUDA::CudaContext& cudaContext, CUDA::CudaTexture2D& cudaTexture)
{
    if (!cudaTexture.textureResource) { return; }
    if (cudaTexture.isMemMapped) {
        throw std::runtime_error("Unmap the texture before unregistering it");
    }
    // Unregister With Cuda
    CUDA_ASSERT(cudaSetDevice(cudaContext.cudaDevice));
    CUDA_ASSERT(cudaGraphicsUnregisterResource(cudaTexture.textureResource));
    cudaTexture.textureResource = nullptr;
    cudaTexture.textureDesc = {};
}

/*
    Functions to Fetch OpenGL Resources In Cuda
*/

// Create Writtable Object so Cuda Kernels can Write to OpenGL Texture
static void MemMapTexture(CUDA::CudaContext& cudaContext, CUDA::CudaTexture2D& cudaTexture) 
{
    CUDA_ASSERT(cudaSetDevice(cudaContext.cudaDevice));
    CUDA_ASSERT(cudaGraphicsMapResources(
        1,
        &cudaTexture.textureResource,
        0
    ));
    cudaTexture.isMemMapped = true;
    cudaTexture.textureDesc.resType = cudaResourceTypeArray;

    CUDA_ASSERT(cudaGraphicsSubResourceGetMappedArray(
        &cudaTexture.textureDesc.res.array.array,
        cudaTexture.textureResource,
        0,  // Array layer
        0   // Mipmap level
    ));

    CUDA_ASSERT(cudaCreateSurfaceObject(
        &cudaTexture.surface,
        &cudaTexture.textureDesc
    ));
}

// Return Control of the Texture to OpenGL After Cuda Kernel Finishes
static void UnMemMapTexture(const CUDA::CudaContext& cudaContext, CUDA::CudaTexture2D& cudaTexture)
{
    if (!cudaTexture.isMemMapped) { return; }

    CUDA_ASSERT(cudaSetDevice(cudaContext.cudaDevice));
    CUDA_ASSERT(cudaDeviceSynchronize());
    
    if (cudaTexture.surface) {
        CUDA_ASSERT(cudaDestroySurfaceObject(cudaTexture.surface));
        cudaTexture.surface = 0;
    }

    CUDA_ASSERT(cudaGraphicsUnmapResources(1, &cudaTexture.textureResource, 0));
    cudaTexture.isMemMapped = false;
    cudaTexture.textureDesc = {};
}

};

