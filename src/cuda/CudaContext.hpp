#pragma once

#include <GL/glew.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <driver_types.h>

#include "CrrtErrors.hpp"

/*
    Info on Cuda Device
*/

namespace CRRT::CUDA {

struct CudaContext
{
    // Cuda Device Informatiaon
    unsigned int cudaDeviceCount = 0;
    int cudaDevice = -1;
};

[[nodiscard]] Errors FetchCudaContext(CudaContext& context) noexcept;

}

