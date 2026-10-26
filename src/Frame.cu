#include "../include/CRRT.hpp"

#include <surface_indirect_functions.h>

namespace CRRT {

// IMPORTANT: Make sure to use BeginCudaAccess() before attempting to read or write to frame in kernel
__device__ void WriteFrame(FrameView frame, int x, int y, uchar4 color)
{
    surf2Dwrite(color, frame.surface, x * sizeof(uchar4), y);
}

__device__ uchar4 ReadFrame(FrameView frame, int x, int y)
{
    return surf2Dread<uchar4>(frame.surface, x * sizeof(uchar4), y);
}

};
