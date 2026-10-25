#pragma once

#include <cstdio>
#include <string> 

#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <driver_types.h>

/*
    Cuda Related Errors
*/

namespace CRRT { namespace CUDA {

// No need to make your own error enum and toString() because cuda already made it
using CudaError = cudaError_t;
inline std::string CudaGetErrorString(CudaError error) 
{
    return std::string(cudaGetErrorString(error));
}

// Macro to return the error code inside a function if a function call fails
#define CUDA_CHECK_OR_RETURN(call)                               \
    do {                                                        \
        const auto cudaCheckResult = (call);                     \
        if (cudaCheckResult != cudaSuccess) {                    \
            return cudaCheckResult;                             \
        }                                                       \
    } while (false)

}};
