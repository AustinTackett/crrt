#include "CudaTexture2D.hpp"
#include <surface_indirect_functions.h>

namespace CUDA {

__device__ void write(CudaTexture2D cudaTexture, int x, int y, uchar4 color)
{
    surf2Dwrite(color, cudaTexture.surface, x * sizeof(uchar4), y);
}

__device__ uchar4 read(CudaTexture2D cudaTexture, int x, int y)
{
    return surf2Dread<uchar4>(cudaTexture.surface, x * sizeof(uchar4), y);
}

};
