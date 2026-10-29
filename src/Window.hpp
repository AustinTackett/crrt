#pragma once

#include "../include/Crrt.hpp"

#include <functional>
#include <tuple>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cuda/CudaContext.hpp"
#include "cuda/CudaTexture2D.hpp"
#include "openGL/GlTexture2D.hpp"
#include "openGL/GlShader.hpp"
#include "openGL/GlVao.hpp"

/*
    Functions for Opening, Closing, and Modifying a GLFW Window
*/

namespace CRRT {

using MouseButtonCallbackFn = void (*)(GLFWwindow*, int, int, int);
using KeyCallbackFn         = void (*)(GLFWwindow*, int, int, int, int);
using CursorCallback        = void (*)(GLFWwindow*, double, double);
using DrawCallback          = std::function<void(FrameView)>;

constexpr GLFWwindow* EMPTY_WINDOW = nullptr;

/*
    Window Data Containers
*/

/*

From: include/Crrt.hpp

struct WindowConfig
{
    std::string name = "Window";
    
    int resWidth   = 1920;
    int resHeight  = 1080;
    int initWidth  = 500;
    int initHeight = 500;

    bool clearColor = true;
    bool resizable  = true;
    bool vsync      = true;
    bool cursorVisible = true;
};

*/

struct Window
{
    static inline GLFWwindow* window = nullptr;

    static inline CUDA::CudaContext   cudaContext{};
    static inline CUDA::CudaTexture2D cudaTexture{};
    static inline GL::GlVao           glVao{};
    static inline GL::GlTexture2D     glTexture{};
    static inline GL::GlShader        glShader{};

    static inline bool cursorVisible = true;
    static inline bool clearColor    = true;
};

/*
    Forward Declarations
*/

[[nodiscard]] Errors CreateWindow(const WindowConfig& windowConfig);
[[nodiscard]] Errors UpdateWindow(DrawCallback drawCallback);
[[nodiscard]] Errors ReleaseWindow();

FrameView GetFrameView();
void SwapBuffer();
bool ShouldClose();
void SetShouldClose();
void CursorVisibility(bool isVisible);

std::tuple<int, int>       GetSize();
std::tuple<double, double> GetCursorPosition();

void AttachInputCallbacks(KeyCallbackFn keyCallback, MouseButtonCallbackFn mouseButtonCallback);
void PollEvents();

}
