#pragma once


#include <GL/glew.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <driver_types.h>

#include "CrrtErrors.hpp"

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

[[nodiscard]] inline Errors FetchCudaContext(CudaContext& context) noexcept
{
    context = CudaContext{};
    CudaContext candidate;
    Errors errors;
    
    if (cudaGLGetDevices(
        &candidate.cudaDeviceCount,
        &candidate.cudaDevice,
        1,
        cudaGLDeviceListAll))
    {
        errors |= ErrorCode::cudaDeviceQueryFailed;
        return errors;
    }
    if (candidate.cudaDeviceCount == 0) {
        errors |= ErrorCode::cudaNoCompatibleDevice;
        return errors;
    }
    if(cudaSetDevice(candidate.cudaDevice)) {
        errors |= ErrorCode::cudaSetDeviceFailed;
        return errors;
    }

    context = candidate;
    return errors;
}

}};

