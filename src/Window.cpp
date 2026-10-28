#include "Window.hpp"

namespace CRRT {

static void AttachInputCallbacks(KeyCallbackFn keyCallback, MouseButtonCallbackFn mouseButtonCallback);
static void PollEvents();

[[nodiscard]] Errors CreateWindow(const WindowConfig& windowConfig)
{
    Errors errors;
    if (Window::window || Window::frame)
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

    Window::window = glfwCreateWindow(
        windowConfig.initWidth,
        windowConfig.initHeight,
        windowConfig.name.c_str(),
        nullptr,
        nullptr);
    if (!Window::window) {
        errors |= ErrorCode::windowsCreationFailed;
        glfwTerminate();
        return errors;
    }
    
    glfwMakeContextCurrent(Window::window);
    if (glfwGetCurrentContext() != Window::window) {
        errors |= ErrorCode::windowsContextActivationFailed;
        errors |= ReleaseWindow();
        return errors;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        errors |= ErrorCode::windowsGlewInitializationFailed;
        errors |= ReleaseWindow();
        return errors;
    }

    Window::frame = new (std::nothrow) Frame{};
    if (!Window::frame) {
        errors |= ErrorCode::windowsFrameOperationFailed;
        errors |= ReleaseWindow();
        return errors;
    }

    errors |= CreateFrame(*Window::frame, windowConfig.resWidth, windowConfig.resHeight);
    if (errors) {
        errors |= ErrorCode::windowsFrameOperationFailed;
        errors |= ReleaseWindow();
        return errors;
    }

    Window::clearColor = windowConfig.clearColor;
    glfwSwapInterval(windowConfig.vsync);
    CursorVisibility(windowConfig.cursorVisible);
    AttachInputCallbacks(KeyCallback, MouseButtonCallback);
    
    PollEvents();
    StartTime();
    return errors;
}

[[nodiscard]] Errors UpdateWindow(DrawCallback drawCallback)
{
    Errors errors;
    if (!Window::window || !Window::frame) {
        errors |= ErrorCode::windowsInvalidWindow;
        return errors;
    }
    
    if (glfwGetCurrentContext() != Window::window)
    {
        errors |= ErrorCode::windowsContextActivationFailed;
        return errors;
    }

    PollEvents();
    UpdateTime();

    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(Window::window, &width, &height);;
    glViewport(0, 0, width, height);

    if (Window::clearColor) { 
        glClear(GL_COLOR_BUFFER_BIT); 
    } 

    if (!drawCallback) {
        Draw(*Window::frame);
        return errors;
    }

    // Pass OpenGL texture to Cuda
    errors |= BeginCudaAccess(*Window::frame);
    if (errors) {
        errors |= ErrorCode::windowsFrameOperationFailed;
        return errors;
    }

    // Run user draw callback
    try {
        drawCallback(GetFrameView(*Window::frame));
    } catch (...) {
        errors |= ErrorCode::windowDrawCallbackException;
        errors |= ErrorCode::userUncaughtException;
    }

    // Return Access of the texture to OpenGL
    Errors cleanUpErrors = EndCudaAccess(*Window::frame);
    if (cleanUpErrors) { 
        errors |= cleanUpErrors | ErrorCode::windowsFrameCleanupFailed; 
    }
    
    // Draw texture to screen
    if (!errors) { 
        Draw(*Window::frame); 
    }

    return errors;
}

[[nodiscard]] Errors ReleaseWindow()
{
    Errors errors;
    if (!Window::window && !Window::frame) {
        return errors;
    }
    if (!Window::window) {
        errors |= ErrorCode::windowsInvalidWindow;
        return errors;
    }


    if (glfwGetCurrentContext() != Window::window) {
        glfwMakeContextCurrent(Window::window);
        if (glfwGetCurrentContext() != Window::window) {
            errors |= ErrorCode::windowsContextActivationFailed;
            return errors;
        }
    }

    // Clear any frame data, but don't delete the frame reference if resources weren't cleared
    if (Window::frame) {
        errors |= ReleaseFrame(*Window::frame);
        if (IsFrameAlive(*Window::frame)) {
            errors |= ErrorCode::windowsFrameCleanupFailed;
            return errors;
        }
        delete Window::frame;
        Window::frame = nullptr;
    }

    glfwDestroyWindow(Window::window);
    Window::window = nullptr;
    glfwTerminate();

    return errors;
}

static void AttachInputCallbacks(KeyCallbackFn keyCallback, MouseButtonCallbackFn mouseButtonCallback)
{
    if (!Window::window) { return; }

    glfwSetKeyCallback(Window::window, keyCallback);
    glfwSetMouseButtonCallback(Window::window, mouseButtonCallback);
}

static void PollEvents()
{
    glfwPollEvents();
    auto [cursorX, cursorY] = GetCursorPosition();
    MouseUpdate(cursorX, cursorY);
}

void SwapBuffer()
{
    if (!Window::window) { return; }
    glfwSwapBuffers(Window::window);
}

bool ShouldClose()
{
    if (!Window::window) { return true; }
    bool shouldClose = glfwWindowShouldClose(Window::window);
    return shouldClose;
}

void SetShouldClose()
{
    if (!Window::window) { return;}
    glfwSetWindowShouldClose(Window::window, true);
}

std::tuple<double, double> GetCursorPosition()
{
    if (!Window::window) { return std::tuple<double, double>(0.0, 0.0); }
    double x = 0.0, y = 0.0;
    glfwGetCursorPos(Window::window, &x, &y);
    return std::tuple<double, double>(x, y);
}

std::tuple<int, int> GetSize()
{
    if (!Window::window) { return std::tuple<int, int>(0, 0); }
    int width, height;
    glfwGetWindowSize(Window::window, &width, &height);
    return std::tuple<int, int>(width, height);
}

void CursorVisibility(bool isVisible)
{
    if (!Window::window) { return; }
    
    if(isVisible) {
        glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        Window::cursorVisible = true;
    } else {
        glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        Window::cursorVisible = false;
    }
}

}
