#include <cmath>
#include <iostream>
#include <cuda_runtime.h>

#include "DebugKernel.hpp"

// Draw Colors to Frame
__global__ void DebugColors(CRRT::FrameView frame, float color)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= frame.width || y >= frame.height) return;

    float4 rgba = make_float4(
        color,
        static_cast<float>(x) / frame.width,
        static_cast<float>(y) / frame.height,
        1.0f
    );

    // Indicate data is written as linear rgb
    CRRT::WriteFrameRgb(frame, x, y, rgba); 
}

void RenderDebugColors(CRRT::FrameView frame)
{
    // Launch Kernel
    dim3 gridDim((frame.width + 15) / 16, (frame.height + 15) / 16);
    dim3 blockDim(16, 16);
    
    float color = 0.5f * std::sin(CRRT::Time::CurrentTime) + 0.5f;
    DebugColors<<<gridDim, blockDim>>>(frame, color);
    
    // It is ok to throw errors, the update window function catches them
    cudaError_t error = cudaGetLastError();
    if (error != cudaSuccess) {
        std::cout << cudaGetErrorString(error) << "\n";
        throw std::runtime_error("My User Exception");
    }

}
