
#include "../include/Crrt.hpp"

#include <cuda_runtime.h>
#include <surface_indirect_functions.h>

namespace CRRT {

/*
   Color conversion
*/

__device__ float LinearToSrgb(float x)
{
    x = fminf(fmaxf(x, 0.0f), 1.0f);
    return x <= 0.0031308f
        ? 12.92f * x
        : 1.055f * powf(x, 1.0f / 2.4f) - 0.055f;
}

__device__ float SrgbToLinear(float x)
{
    x = fminf(fmaxf(x, 0.0f), 1.0f);
    return x <= 0.04045f
        ? x / 12.92f
        : powf((x + 0.055f) / 1.055f, 2.4f);
}

__device__ float4 RgbToSrgb(float4 color)
{
    return make_float4(
        LinearToSrgb(color.x),
        LinearToSrgb(color.y),
        LinearToSrgb(color.z),
        color.w
    );
}

__device__ float4 SrgbToRgb(float4 color)
{
    return make_float4(
        SrgbToLinear(color.x),
        SrgbToLinear(color.y),
        SrgbToLinear(color.z),
        color.w
    );
}

/*
   RGBA8 packing / unpacking
*/

__device__ float4 UnpackRgba8(uchar4 color)
{
    constexpr float scale = 1.0f / 255.0f;

    return make_float4(
        color.x * scale,
        color.y * scale,
        color.z * scale,
        color.w * scale
    );
}

__device__ __forceinline__ unsigned char PackChannel(float x)
{
    x = fminf(fmaxf(x, 0.0f), 1.0f);
    return static_cast<unsigned char>(x * 255.0f + 0.5f);
}

__device__ uchar4 PackRgba8(float4 color)
{
    return make_uchar4(
        PackChannel(color.x),
        PackChannel(color.y),
        PackChannel(color.z),
        PackChannel(color.w)
    );
}

/*
   Raw surface access
   Coordinates are NOT bounds checked.
*/

__device__ void WriteFrameRaw(FrameView frame, int x, int y, uchar4 color)
{
    surf2Dwrite(color, frame.surface, x * sizeof(uchar4), y);
}

__device__ uchar4 ReadFrameRaw(FrameView frame, int x, int y)
{
    return surf2Dread<uchar4>(
        frame.surface, x * sizeof(uchar4), y
    );
}

__device__ uchar4 ReadFrameRawOrZero(FrameView frame, int x, int y)
{
    return surf2Dread<uchar4>(
        frame.surface, x * sizeof(uchar4), y,
        cudaBoundaryModeZero
    );
}

/*
   sRGB access
   RGB is already sRGB encoded.
   Alpha remains linear.
*/

__device__ void WriteFrameSrgb(FrameView frame, int x, int y, float4 color)
{
    WriteFrameRaw(frame, x, y, PackRgba8(color));
}

__device__ float4 ReadFrameSrgb(FrameView frame, int x, int y)
{
    return UnpackRgba8(ReadFrameRaw(frame, x, y));
}

__device__ float4 ReadFrameSrgbOrZero(FrameView frame, int x, int y)
{
    return UnpackRgba8(ReadFrameRawOrZero(frame, x, y));
}

/*
   Linear RGB access
   Converts between linear RGB and sRGB storage.
   Alpha remains linear.
*/

__device__ void WriteFrameRgb(FrameView frame, int x, int y, float4 color)
{
    WriteFrameSrgb(frame, x, y, RgbToSrgb(color));
}

__device__ float4 ReadFrameRgb(FrameView frame, int x, int y)
{
    return SrgbToRgb(ReadFrameSrgb(frame, x, y));
}

__device__ float4 ReadFrameRgbOrZero(FrameView frame, int x, int y)
{
    return SrgbToRgb(ReadFrameSrgbOrZero(frame, x, y));
}

};
