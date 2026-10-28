#pragma once

#include "../include/Crrt.hpp"

#include <functional>
#include <string>
#include <tuple>
#include <new>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include "Frame.hpp"
#include "Input.hpp"
#include "Time.hpp"

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

*/

/*
    Creating Window and Releasing Window
*/

// Forward Declarations
[[nodiscard]] Errors ReleaseWindow(Window& window);
static void AttachInputCallbacks(Window& window, KeyCallbackFn keyCallback, MouseButtonCallbackFn mouseButtonCallback);
static void PollEvents(Window& window);
void SwapBuffer(Window& window);
bool ShouldClose(Window& window);
void SetShouldClose(Window& window);
void CursorVisibility(Window& window, bool isVisible);
std::tuple<int, int>       GetSize(Window& window);
std::tuple<double, double> GetCursorPosition(Window& window);

// Failed creation releases owned resources unless CUDA cleanup must be retried.
[[nodiscard]] Errors CreateWindow(Window& window, const WindowConfig& windowConfig)
{
    Errors errors;
    if (window.window || window.frame)
    {
        errors |= ErrorCode::windowsAlreadyCreated;
        return errors;
    }
    if (windowConfig.initWidth <= 0 || windowConfig.initHeight <= 0 ||
        windowConfig.resWidth <= 0 || windowConfig.resHeight <= 0)
    {
        errors |= ErrorCode::windowsInvalidWindow;
        return errors;
    }

    if (!glfwInit()) {
        errors |= ErrorCode::windowsGlfwInitializationFailed;
        return errors;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, windowConfig.resizable ? GLFW_TRUE : GLFW_FALSE);

    window.window = glfwCreateWindow(
        windowConfig.initWidth,
        windowConfig.initHeight,
        windowConfig.name.c_str(),
        nullptr,
        nullptr);
    if (!window.window) {
        errors |= ErrorCode::windowsCreationFailed;
        glfwTerminate();
        return errors;
    }
    
    glfwMakeContextCurrent(window.window);
    if (glfwGetCurrentContext() != window.window) {
        errors |= ErrorCode::windowsContextActivationFailed;
        glfwDestroyWindow(window.window);
        window.window = EMPTY_WINDOW;
        glfwTerminate();
        return errors;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        errors |= ErrorCode::windowsGlewInitializationFailed;
        glfwDestroyWindow(window.window);
        window.window = EMPTY_WINDOW;
        glfwTerminate();
        return errors;
    }

    window.frame = new (std::nothrow) Frame{};
    if (!window.frame) {
        errors |= ErrorCode::windowsFrameOperationFailed;
        glfwDestroyWindow(window.window);
        window.window = EMPTY_WINDOW;
        glfwTerminate();
        return errors;
    }

    errors = CreateFrame(*window.frame, windowConfig.resWidth, windowConfig.resHeight);
    if (errors) {
        errors |= ErrorCode::windowsFrameOperationFailed;
        Errors cleanupErrors = ReleaseFrame(*window.frame);
        errors |= cleanupErrors;
        if (cleanupErrors) {
            errors |= ErrorCode::windowsFrameCleanupFailed;
        }

        delete window.frame;
        window.frame = nullptr;
        glfwDestroyWindow(window.window);
        window.window = EMPTY_WINDOW;
        glfwTerminate();
        return errors;
    }

    window.clearColor = windowConfig.clearColor;
    glfwSwapInterval(windowConfig.vsync ? 1 : 0);
    CursorVisibility(window, windowConfig.cursorVisible);
    AttachInputCallbacks(window, KeyCallback, MouseButtonCallback);
    
    PollEvents(window);
    StartTime();
    return errors;
}

[[nodiscard]] Errors UpdateWindow(Window& window, DrawCallback drawCallback)
{
    Errors errors;
    if (!window.window || !window.frame) {
        errors |= ErrorCode::windowsInvalidWindow;
        return errors;
    }

    // Update Window Size, Time, and User Inputs
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window.window, &width, &height);;
    glViewport(0, 0, width, height);
    PollEvents(window);
    UpdateTime();
    if (window.clearColor) { glClear(GL_COLOR_BUFFER_BIT); } 
    if (!drawCallback) {
        Draw(*window.frame);
        return errors;
    }

    // Pass OpenGL texture to Cuda
    errors = BeginCudaAccess(*window.frame);
    if (errors) {
        errors |= ErrorCode::windowsFrameOperationFailed;
        return errors | EndCudaAccess(*window.frame);
    }

    // Run user draw callback
    try {
        drawCallback(GetFrameView(*window.frame));
    } catch (...) {
        errors |= ErrorCode::windowDrawCallbackException;
        errors |= ErrorCode::userUncaughtException;
    }

    // Return Access of the texture to OpenGL
    errors |= EndCudaAccess(*window.frame);
    if (errors) { 
        errors |= ErrorCode::windowsFrameCleanupFailed; 
    }
    
    // Draw texture to screen
    if (!errors) { 
        Draw(*window.frame); 
    }

    return errors;
}

[[nodiscard]] Errors ReleaseWindow(Window& window)
{
    Errors errors;
    if (!window.window && !window.frame) {
        return errors;
    }

    errors = ReleaseFrame(*window.frame);
    if (errors) { 
        errors |= ErrorCode::windowsFrameCleanupFailed; 
    }

    glfwDestroyWindow(window.window);
    window.window = EMPTY_WINDOW;

    delete window.frame;
    window.frame = nullptr;

    glfwTerminate();
    return errors;
}

/*
    Handling User IO 
*/

static void AttachInputCallbacks(Window& window, KeyCallbackFn keyCallback, MouseButtonCallbackFn mouseButtonCallback)
{
    if (!window.window) { return; }
    glfwSetKeyCallback(window.window, keyCallback);
    glfwSetMouseButtonCallback(window.window, mouseButtonCallback);
}

static void PollEvents(Window& window)
{
    glfwPollEvents();
    auto [cursorX, cursorY] = GetCursorPosition(window);
    MouseUpdate(cursorX, cursorY);
}

/*
    Utilities 
*/

void SwapBuffer(Window &window)
{
    if (!window.window) { return; }
    glfwSwapBuffers(window.window);
}

bool ShouldClose(Window& window)
{
    if (!window.window) { return true; }
    bool shouldClose = glfwWindowShouldClose(window.window);
    return shouldClose;
}

void SetShouldClose(Window& window)
{
    if (!window.window) { return;}
    glfwSetWindowShouldClose(window.window, true);
}

std::tuple<double, double> GetCursorPosition(Window& window)
{
    if (!window.window) { return std::tuple<double, double>(0.0, 0.0); }
    double x = 0.0, y = 0.0;
    glfwGetCursorPos(window.window, &x, &y);
    return std::tuple<double, double>(x, y);
}

std::tuple<int, int> GetSize(Window& window)
{
    if (!window.window) { return std::tuple<int, int>(0, 0); }
    int width, height;
    glfwGetWindowSize(window.window, &width, &height);
    return std::tuple<int, int>(width, height);
}

void CursorVisibility(Window& window, bool isVisible)
{
    if (!window.window) { return; }
    
    if(isVisible) {
        glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        window.cursorVisible = true;
    } else {
        glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        window.cursorVisible = false;
    }
}

};



