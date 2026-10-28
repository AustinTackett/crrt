#pragma once

#include <functional>
#include <string>
#include <tuple>

#include <cuda_runtime.h>

#include "CrrtErrors.hpp"

struct GLFWwindow;

namespace CRRT {

/*
    Frame that stores pixels
*/

struct Frame;  

struct FrameView
{
    cudaSurfaceObject_t surface = 0;
    int width = 0;
    int height = 0;
};

/*
    Functions for drawing to frame
    -> Draw callback needs to be a function that takes FrameView as an arguement and returns void
*/

using DrawCallback = std::function<void(FrameView)>;
__device__ void WriteFrame(FrameView frame, int x, int y, uchar4 color);
__device__ uchar4 ReadFrame(FrameView frame, int x, int y);

/*
    Window Start up configuration
*/

struct Window
{
    GLFWwindow* window = nullptr;
    Frame*      frame  = nullptr;

    bool cursorVisible = true;
    bool clearColor = true;
};

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

[[nodiscard]] Errors CreateWindow(Window& window, const WindowConfig& windowConfig);
[[nodiscard]] Errors UpdateWindow(Window& window, DrawCallback drawCallback = nullptr);
[[nodiscard]] Errors ReleaseWindow(Window& window);

std::tuple<double, double> GetCursorPosition(Window& window);
std::tuple<int, int> GetSize(Window& window);
void SwapBuffer(Window& window);
bool ShouldClose(Window& window);
void SetShouldClose(Window& window);
void CursorVisibility(Window& window, bool isVisible);

/*
    Tracking time
*/

struct Time
{
    // Seconds
    inline static double CurrentTime;
    inline static double DeltaTime = 0.0f;
};

/*
    Track what current inputs are being pressed
*/

struct Input
{
    // Special Keys
    inline static bool KEY_ESCAPE         = false;
    inline static bool KEY_ENTER          = false;
    inline static bool KEY_TAB            = false;
    inline static bool KEY_CONTROL        = false;
    inline static bool KEY_SPACE          = false;
    inline static bool KEY_SHIFT          = false;
    inline static bool KEY_ALT            = false;
    
    // Arrow Keys
    inline static bool KEY_UP    = false;
    inline static bool KEY_DOWN  = false;
    inline static bool KEY_LEFT  = false;
    inline static bool KEY_RIGHT = false;

    // Mouse
    inline static bool   MOUSE_RIGHT_CLICK = false;    
    inline static bool   MOUSE_LEFT_CLICK  = false;
    inline static double MOUSE_X_DELTA     = 0.0;
    inline static double MOUSE_Y_DELTA     = 0.0;
    inline static double MOUSE_X           = 0.0;
    inline static double MOUSE_Y           = 0.0;

    // Numeric Keys
    inline static bool KEY_0 = false;
    inline static bool KEY_1 = false;
    inline static bool KEY_2 = false;
    inline static bool KEY_3 = false;
    inline static bool KEY_4 = false;
    inline static bool KEY_5 = false;
    inline static bool KEY_6 = false;
    inline static bool KEY_7 = false;
    inline static bool KEY_8 = false;
    inline static bool KEY_9 = false;

    // Alphabetical Keys
    inline static bool KEY_W = false;
    inline static bool KEY_A = false;
    inline static bool KEY_S = false;
    inline static bool KEY_D = false;
    inline static bool KEY_B = false;
    inline static bool KEY_C = false;
    inline static bool KEY_E = false;
    inline static bool KEY_F = false;
    inline static bool KEY_G = false;
    inline static bool KEY_H = false;
    inline static bool KEY_I = false;
    inline static bool KEY_J = false;
    inline static bool KEY_K = false;
    inline static bool KEY_L = false;
    inline static bool KEY_M = false;
    inline static bool KEY_N = false;
    inline static bool KEY_O = false;
    inline static bool KEY_P = false;
    inline static bool KEY_Q = false;
    inline static bool KEY_R = false;
    inline static bool KEY_T = false;
    inline static bool KEY_U = false;
    inline static bool KEY_V = false;
    inline static bool KEY_X = false;
    inline static bool KEY_Y = false;
    inline static bool KEY_Z = false;
};


};
