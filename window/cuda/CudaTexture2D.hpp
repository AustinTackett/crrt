#pragma once

#include <cuda_runtime.h>

namespace CUDA {

struct CudaTexture2D
{
    cudaGraphicsResource*  textureResource = nullptr;
    cudaResourceDesc       textureDesc{};
    cudaSurfaceObject_t    surface = 0;
    bool                   isMemMapped = false;
};

// IMPORTANT: Make sure texture is MEMMAPPED AND REGISTERED into Cuda Context BEFORE calling these in Cuda Kernels

// Functions to Read/Write to Texture in Cuda Kernel
__device__ void   writeTexture(CudaTexture2D& cudaTexture, int x, int y, uchar4 color);
__device__ uchar4 readTexture(CudaTexture2D&  cudaTexture, int x, int y);

};