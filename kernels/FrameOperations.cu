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
// return uchar4 casted to float4
__device__ float4 ReadFrameFloat(FrameView frame, int x, int y)
{
    uchar4 c = ReadFrame(frame, x, y);
    constexpr float scale = 1.0f / 255.0f;
    return make_float4(c.x * scale, c.y * scale, c.z * scale, c.w * scale);
}

// Not Bound Checked
// Round float4 [0, 1] to uchar4 [0, 255] and write
__device__ void WriteFrameFloat(FrameView frame, int x, int y, float4 color)
{
    color.x = fminf(fmaxf(color.x, 0.0f), 1.0f);
    color.y = fminf(fmaxf(color.y, 0.0f), 1.0f);
    color.z = fminf(fmaxf(color.z, 0.0f), 1.0f);
    color.w = fminf(fmaxf(color.w, 0.0f), 1.0f);
    WriteFrame(frame, x, y, make_uchar4(
        static_cast<unsigned char>(color.x * 255.0f + 0.5f),
        static_cast<unsigned char>(color.y * 255.0f + 0.5f),
        static_cast<unsigned char>(color.z * 255.0f + 0.5f),
        static_cast<unsigned char>(color.w * 255.0f + 0.5f)
    ));
}

};
