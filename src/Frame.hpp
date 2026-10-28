#pragma once

#include "../include/Crrt.hpp"

#include "CudaTexture2D.hpp"
#include "GlShader.hpp"
#include "GlVao.hpp"

namespace CRRT {

/*
    Stores all the data of the pixels on screen, can begin draw pipeline to draw it to the screen, and 
    has functionality to open and close access of the pixel data to cuda.
*/

struct Frame
{
    CUDA::CudaContext   cudaContext;
    CUDA::CudaTexture2D cudaTexture;
    GL::GlVao           glVao;
    GL::GlTexture2D     glTexture;
    GL::GlShader        glShader;
};


/*

From: include/Crrt.hpp

struct FrameView
{
    cudaSurfaceObject_t surface = 0;
    int width = 0;
    int height = 0;
};

*/

[[nodiscard]] Errors CreateFrame(Frame& frame, int width, int height);
[[nodiscard]] Errors ReleaseFrame(Frame& frame);

FrameView GetFrameView(const Frame& frame);

void Draw(Frame& frame);

[[nodiscard]] bool IsFrameAlive(const Frame& frame);

[[nodiscard]] Errors BeginCudaAccess(Frame& frame);
[[nodiscard]] Errors EndCudaAccess(Frame& frame);


};

