#pragma once

#include "../include/CRRT.hpp"

#include <functional>


#include <string>
#include <tuple>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include "ErrorCodes.hpp"
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
    Returning Errors
*/

struct WindowError
{
    bool valid = true;
    WindowErrorCode window = WindowErrorCode::windowsSuccess;
    CUDA::CudaError cuda   = CUDA::CudaError::cudaSuccess;
    GL::GlError gl         = GL::GlError::glSuccess;
};

WindowError CreateWindowError(bool valid, WindowErrorCode code, const FrameError& frameError) {
    return WindowError{false, code, frameError.cuda, frameError.gl};
}

bool hasWindowError(const WindowError& error)
{
    return !error.valid;
}

inline std::string GetWindowErrorString(const WindowError& error)
{
    std::string message = GetWindowErrorString(error.window);
    if (error.cuda != cudaSuccess) {
        message += "\nCUDA: " + CUDA::CudaGetErrorString(error.cuda);
    }
    if (error.gl != GL::GlError::glSuccess) {
        message += "\nOpenGL: " + GL::glGetErrorString(error.gl);
    }

    return message;
}

/*
    Window Data Containers
*/

struct Window 
{
    GLFWwindow* window = EMPTY_WINDOW;
    Frame       frame;

    bool cursorVisible = true;
    bool clearColor    = true;
};


// Forward Declarations
[[nodiscard]] WindowError ReleaseWindow(Window& window);
static void AttachInputCallbacks(Window& window, KeyCallbackFn keyCallback, MouseButtonCallbackFn mouseButtonCallback);
static void PollEvents(Window& window);
static void SwapBuffer(Window& window);
bool ShouldClose(Window& window);
void SetShouldClose(Window& window);
void CursorVisibility(Window& window, bool isVisible);
std::tuple<int, int>       GetSize(Window& window);
std::tuple<double, double> GetCursorPosition(Window& window);

/*
    Creating Window and Releasing Window
*/

// Creation leaves partial resources in window for the caller to release.
[[nodiscard]] WindowError CreateWindow(Window& window, const WindowConfig& windowConfig)
{
    if (GLFW_TRUE != glfwInit()) return {false, WindowErrorCode::windowsGlfwInitializationFailed};

    glfwWindowHint(GLFW_RESIZABLE, windowConfig.resizable ? GLFW_TRUE : GLFW_FALSE);
    window.clearColor = windowConfig.clearColor;
    window.window = glfwCreateWindow(
        windowConfig.initWidth, 
        windowConfig.initHeight,
        windowConfig.name.c_str(), 
        nullptr, 
        nullptr);
    if (!window.window) return {false, WindowErrorCode::windowsCreationFailed};

    glfwMakeContextCurrent(window.window);
    if (glfwGetCurrentContext() != window.window) 
        return {false, WindowErrorCode::windowsContextActivationFailed};

    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) return {false, WindowErrorCode::windowsGlewInitializationFailed};

    FrameError frameError = CreateFrame(
        window.frame, 
        windowConfig.resWidth, 
        windowConfig.resHeight);
    if (hasFrameError(frameError)) return CreateWindowError(false, WindowErrorCode::windowsFrameOperationFailed, frameError);

    glfwSwapInterval(windowConfig.vsync);
    CursorVisibility(window, windowConfig.cursorVisible);
    AttachInputCallbacks(window, KeyCallback, MouseButtonCallback);
    PollEvents(window);
    StartTime();

    return {};
}

[[nodiscard]] WindowError UpdateWindow(Window& window, DrawCallback drawCallback = nullptr)
{
    // Update everything short of swapping buffer
    if (!window.window) return {false, WindowErrorCode::windowsInvalidWindow};

    glfwMakeContextCurrent(window.window);
    if (glfwGetCurrentContext() != window.window) return {false, WindowErrorCode::windowsContextActivationFailed};

    auto [width, height] = GetSize(window);
    glViewport(0, 0, width, height);
    PollEvents(window);
    UpdateTime();

    if (window.clearColor) glClear(GL_COLOR_BUFFER_BIT);
    
    // USERS CUDA KERNEL from here on out
    if (!drawCallback) { return WindowError{}; } 

    // Expose frame to Cuda and if an error occurs return unrecoverable error
    FrameError beginError = BeginCudaAccess(window.frame);
    if (hasFrameError(beginError)) {
        FrameError cleanupError = EndCudaAccess(window.frame);
        if (hasFrameError(cleanupError)) return CreateWindowError(false, WindowErrorCode::windowsFrameCleanupFailed, cleanupError);
        return CreateWindowError(false, WindowErrorCode::windowsFrameOperationFailed, beginError);
    }

    // I have been try to avoid try catch, but if the user's callback throws a runtime error we need to clean up after
    try {
        drawCallback(GetFrameView(window.frame));
    } catch (...) {
        // User's callback threw an error and we need to clean up and exit (unrecoverable)
        FrameError cleanupError = EndCudaAccess(window.frame);
        if (hasFrameError(cleanupError)) return CreateWindowError(false, WindowErrorCode::windowsFrameCleanupFailed, cleanupError);
        return {false, WindowErrorCode::windowDrawCallbackException};
    }
    
    // Return control back to OpenGL ad if an error occurs return unrecoverable error
    FrameError endError = EndCudaAccess(window.frame);
    if (hasFrameError(endError)) return CreateWindowError(false, WindowErrorCode::windowsFrameOperationFailed, endError);

    // CUDA WAS ABLE TO WRITE TO THE OPENGL RESOURCE AND WE CAN DRAW IT TO THE SCREEN YIPEEEEE!
    Draw(window.frame);
    return WindowError{};
}

[[nodiscard]] WindowError ReleaseWindow(Window& window)
{
    if (!window.window) { return WindowError{}; }

    glfwMakeContextCurrent(window.window);
    if (glfwGetCurrentContext() != window.window) return {false, WindowErrorCode::windowsContextActivationFailed};

    FrameError frameError = ReleaseFrame(window.frame);
    glfwDestroyWindow(window.window);
    window.window = EMPTY_WINDOW;

    if (hasFrameError(frameError)) return CreateWindowError(false, WindowErrorCode::windowsFrameOperationFailed, frameError);
    return WindowError{};
}

// Kill window no matter what is happening, should ReleaseWindow() before termination to clean up frame resources
void TerminateWindow()
{
    glfwTerminate();
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
    auto [cursorX, cursorY] = GetCursorPosition(window);
    MouseUpdate(cursorX, cursorY);
    glfwPollEvents();
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
