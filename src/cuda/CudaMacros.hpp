#pragma once

namespace CRRT { namespace CUDA {

// Macro to return the error code inside a function if a function call fails
#define CUDA_CHECK_OR_RETURN(call)                               \
    do {                                                         \
        const auto cudaCheckResult = (call);                     \
        if (cudaCheckResult != cudaSuccess) {                    \
            return cudaCheckResult;                              \
        }                                                        \
    } while (false)

}}