#include <cmath>
#include <iostream>
#include <cuda_runtime.h>

#include "DebugKernel.hpp"

__global__ void DebugColors(CRRT::FrameView frame, int counter)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= frame.width || y >= frame.height) return;

    uchar4 rgba = make_uchar4(
        x, 
        y,
        counter, 
        255
    );
    
    CRRT::WriteFrame(frame, x, y, rgba);
}

void RenderDebugColors(CRRT::FrameView frame)
{
    int counter = std::fmod(100 * CRRT::Time::CurrentTime, 255);

    dim3 blocks((frame.width + 15) / 16, (frame.height + 15) / 16);
    DebugColors<<<blocks, dim3(16, 16)>>>(frame, counter);
}
