#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include <stdexcept>
#include <string>
#include <tuple>

typedef void (*KeyCallback) (GLFWwindow*, int, int, int, int);
typedef void (*MouseButtonCallback) (GLFWwindow*, int, int, int);
typedef void (*CursorCallback) (GLFWwindow*, double, double);

constexpr GLFWwindow* EMPTY_WINDOW = nullptr;

struct WindowConfig
{
    std::string name = "Window";
    
    int width  = 500;
    int height = 500;

    bool clearColor = true;
    bool resizable  = true;
    bool vsync      = true;
};

struct Window 
{
    GLFWwindow* window = EMPTY_WINDOW;
    bool isCursorHidden = false;
};

static Window CreateWindow(WindowConfig windowConfig)
{   
    if (GLFW_TRUE != glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    Window window;
    window.window = glfwCreateWindow(
        windowConfig.width, 
        windowConfig.height, 
        windowConfig.name.c_str(), 
        NULL, 
        NULL);
    if (!window.window) {
        throw std::runtime_error("Could not create a GLFW window");
    } 
    

    glfwMakeContextCurrent(window.window);
    glfwSwapInterval(1);
    glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    return window;
}

static void ReleaseWindow(Window& window)
{
    if (!window.window) { return; }

    glfwDestroyWindow(window.window);
    glfwTerminate();
    window.window = EMPTY_WINDOW;
    window.isCursorHidden = false;
}

/*
    Display Back Frame Buffer
*/

static void SwapBuffer(Window& window)
{
    if (!window.window) { return; }
    glfwSwapBuffers(window.window);
}

/*
    Handling User IO 
*/

static void AttachInputCallbacks(Window& window, KeyCallback keyCallback, MouseButtonCallback mouseButtonCallback)
{
    if (!window.window) { return; }
    glfwSetKeyCallback(window.window, keyCallback);
    glfwSetMouseButtonCallback(window.window, mouseButtonCallback);
}

static void PollEvents()
{
    glfwPollEvents();
}

/*
    Utilities 
*/

static bool ShouldClose(Window& window)
{
    if (!window.window) { return true; }
    return glfwWindowShouldClose(window.window);
}

static void SetShouldClose(Window& window)
{
    if (!window.window) { return;}
    glfwSetWindowShouldClose(window.window, true);
}

static std::tuple<double, double> GetCursorPosition(Window& window)
{
    if (!window.window) { return std::tuple<double, double>(0.0, 0.0); }
    double x = 0.0, y = 0.0;
    glfwGetCursorPos(window.window, &x, &y);
    return std::tuple<double, double>(x, y);
}

static std::tuple<int, int> GetSize(Window& window)
{
    if (!window.window) { return std::tuple<int, int>(0, 0); }
    int width, height;
    glfwGetWindowSize(window.window, &width, &height);
    return std::tuple<int, int>(width, height);
}

static void CursorVisibility(Window& window, bool isVisible)
{
    if (!window.window) { return; }
    
    if(isVisible) {
        glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        window.isCursorHidden = true;
    } else {
        glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        window.isCursorHidden = true;
    }
}
