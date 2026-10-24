#pragma once

#include <stdexcept>
#include <string>

#include <cuda_runtime.h>

namespace CUDA {

/*
    Utility to throw run time error if cuda function has error
    -> CUDA_CHECK(cuda_function(...));
    -> Throws run time error if cuda_function() has error
*/

#define CUDA_ASSERT(call) { CUDA::cudaAssert((call), __FILE__, __LINE__); }
inline void cudaAssert(cudaError_t error, const char* file, int line)
{
    if (error != cudaSuccess) {
        throw std::runtime_error(
            std::string("CUDA Error: ") + cudaGetErrorString(error) 
            +"\nIn File: " + file + "\nAt Line: " + std::to_string(line)
        );
    }
}

};