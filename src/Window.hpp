#pragma once

#include "../include/Crrt.hpp"

#include <functional>
#include <tuple>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include "Frame.hpp"

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
    static inline Frame*      frame  = nullptr;

    static inline bool cursorVisible = true;
    static inline bool clearColor = true;
};

/*
    Creating Window and Releasing Window
*/

[[nodiscard]] Errors CreateWindow(const WindowConfig& windowConfig);
[[nodiscard]] Errors UpdateWindow(DrawCallback drawCallback);
[[nodiscard]] Errors ReleaseWindow();


/*
    Utilities 
*/

void SwapBuffer();
bool ShouldClose();
void SetShouldClose();
void CursorVisibility(bool isVisible);
std::tuple<int, int>       GetSize();
std::tuple<double, double> GetCursorPosition();

};

