#pragma once

#include <GL/glew.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include "ErrorCodes.hpp"
#include "CudaMacros.hpp"

/*
    Info on Cuda Device
*/

namespace CRRT { namespace CUDA {

struct CudaContext
{
    // Cuda Device Informatiaon
    unsigned int cudaDeviceCount = 0;
    int cudaDevice = -1;
};

[[nodiscard]] CudaError FetchCudaContext(CudaContext& context) noexcept
{
    context = CudaContext{};

    CUDA_CHECK_OR_RETURN(cudaGLGetDevices(
        &context.cudaDeviceCount, 
        &context.cudaDevice,
        1,
        cudaGLDeviceListAll
    ));

    if (context.cudaDeviceCount == 0) {
        return CudaError::cudaErrorNoDevice;
    }
    CUDA_CHECK_OR_RETURN(cudaSetDevice(context.cudaDevice));

    return CudaError::cudaSuccess;
}


}};

