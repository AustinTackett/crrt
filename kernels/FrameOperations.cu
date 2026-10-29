#include "../include/Crrt.hpp"

#include <surface_indirect_functions.h>

namespace CRRT {

// Not Bound Checked
__device__ void WriteFrame(FrameView frame, int x, int y, uchar4 color)
{
    surf2Dwrite(color, frame.surface, x * sizeof(uchar4), y);
}

// Not Bound Checked
__device__ uchar4 ReadFrame(FrameView frame, int x, int y)
{
    return surf2Dread<uchar4>(frame.surface, x * sizeof(uchar4), y);
}

// If out of bounds returns 0
__device__ uchar4 ReadFrameOrZero(FrameView frame, int x, int y)
{
    return surf2Dread<uchar4>(frame.surface, x * sizeof(uchar4), y, cudaBoundaryModeZero);
}

// Not Bound Checked
// Cast uchar to float4
__device__ float4 ReadFrameFloat(FrameView frame, int x, int y)
{
    uchar4 c = ReadFrame(frame, x, y);
    constexpr float scale = 1.0f / 255.0f;
    return make_float4(c.x * scale, c.y * scale, c.z * scale, c.w * scale);
}

};
