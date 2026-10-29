#pragma once

#include "CrrtErrors.hpp"

/*
    Info on Cuda Device
*/

namespace CRRT::CUDA {

struct CudaContext
{
    unsigned int cudaDeviceCount = 0;
    int cudaDevice = -1;
};

[[nodiscard]] Errors FetchCudaContext(CudaContext& context) noexcept;

}

