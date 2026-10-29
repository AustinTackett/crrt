#include "Window.hpp"

#include "Input.hpp"
#include "Time.hpp"

namespace CRRT {

/*
    Creating and Destroying Windows
*/

[[nodiscard]] Errors CreateWindow(const WindowConfig& windowConfig)
{
    // Check Valid Window Parameters
    Errors errors;
    if (Window::window)
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

    // Create GLFW window
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

    // Get OpenGL Function Pointers
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        errors |= ErrorCode::windowsGlewInitializationFailed;
        errors |= ReleaseWindow();
        return errors;
    }

    // Create OpenGL Texture Frame Render Target
    if (Window::cudaTexture.textureResource ||
        Window::cudaTexture.isMemMapped ||
        Window::cudaTexture.surface ||
        Window::glTexture.texture ||
        Window::glShader.shader ||
        Window::glVao.vao) 
    {
        errors |= ErrorCode::frameAlreadyCreated;
        errors |= ReleaseWindow();
        return errors;
    }
    if (windowConfig.resWidth <= 0 || windowConfig.resHeight <= 0) {
        errors |= ErrorCode::glTextureInvalidDimensions;
        errors |= ReleaseWindow();
        return errors;
    }
    GL::GlTexture2DConfig textureConfig;
    textureConfig.width = windowConfig.resWidth;
    textureConfig.height = windowConfig.resHeight;
    errors |= GL::CreateVao(Window::glVao);
    errors |= GL::LoadShader(Window::glShader);
    errors |= GL::CreateEmptyTexture(Window::glTexture, textureConfig);
    if (errors) { 
        errors |= ReleaseWindow();
        return errors; 
    }
    
    errors |= CUDA::FetchCudaContext(Window::cudaContext);
    if (errors) {
        errors |= ReleaseWindow();
        return errors;
    }

    // Register OpenGL Texture With Cuda
    errors |= CUDA::RegisterTextureInterop(
        Window::cudaContext, 
        Window::cudaTexture, 
        Window::glTexture);
    if (errors) {
        errors |= ReleaseWindow();
        return errors;
    }

    // Configure GLFW Window Properties
    Window::clearColor = windowConfig.clearColor;
    glfwSwapInterval(windowConfig.vsync);
    CursorVisibility(windowConfig.cursorVisible);
    AttachInputCallbacks(KeyCallback, MouseButtonCallback);
    
    // Start Time Tracking and Polling Users Inputs
    PollEvents();
    StartTime();
    return errors;
}

[[nodiscard]] Errors UpdateWindow(DrawCallback drawCallback)
{
    // Check Window is In Valid State
    Errors errors;
    if (!Window::window) {
        errors |= ErrorCode::windowsInvalidWindow;
        errors |= ReleaseWindow();
        return errors;
    }
    if (glfwGetCurrentContext() != Window::window)
    {
        errors |= ErrorCode::windowsContextActivationFailed;
        errors |= ReleaseWindow();
        return errors;
    }

    // Update Time and Poll User Inputs
    PollEvents();
    UpdateTime();
    
    // Update Window Viewport and Clear it
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(Window::window, &width, &height);;
    glViewport(0, 0, width, height);
    if (Window::clearColor) { 
        glClear(GL_COLOR_BUFFER_BIT); 
    }

    // Drawing Using OpenGL Current State of OpenGL Texture
    auto draw = []() -> Errors {
        Errors drawErrors;
        try {
            GL::BindShader(Window::glShader);
            GL::BindTexture(Window::glTexture, 0);
            GL::SetUniform1i(Window::glShader, "frameTexture", 0);
            GL::DrawVao(Window::glVao);
        } catch (...) {
            drawErrors |= ErrorCode::windowDrawException;
        }
        return drawErrors;
    };

    // Draw Using User Callback
    if (!drawCallback) {
        errors |= draw();
        if (errors) { 
            errors |= ReleaseWindow(); 
        }
        return errors;
    }
    errors |= CUDA::MemMapTexture(Window::cudaContext, Window::cudaTexture);
    if (errors) {
        errors |= ErrorCode::windowsFrameOperationFailed;
        errors |= ReleaseWindow();
        return errors;
    }
    try {
        drawCallback(GetFrameView());
    } catch (...) {
        errors |= ErrorCode::windowDrawCallbackException;
        errors |= ErrorCode::userUncaughtException;
    }
    Errors cleanUpErrors = CUDA::UnMemMapTexture(Window::cudaContext, Window::cudaTexture);
    errors |= cleanUpErrors;
    if (cleanUpErrors) {
        errors |= ErrorCode::windowsFrameCleanupFailed;
    }
    if (errors) { 
        errors |= ReleaseWindow();
        return errors;
    }

    errors |= draw();
    if (errors) { 
        errors |= ReleaseWindow(); 
    }
    return errors;
}

// Terminal, best-effort shutdown. Failed resources are not retried.
[[nodiscard]] Errors ReleaseWindow()
{
    Errors errors;

    // Make GLFW Window Current Context
    bool hasContext = false;
    if (Window::window) {
        if (glfwGetCurrentContext() != Window::window) {
            glfwMakeContextCurrent(Window::window);
        }
        hasContext = glfwGetCurrentContext() == Window::window;
        if (!hasContext) {
            errors |= ErrorCode::windowsContextActivationFailed;
        }
    }

    // Attempt to UnMemMap and UnRegister OpenGL resources from CUDA
    errors |= CUDA::UnMemMapTexture(Window::cudaContext, Window::cudaTexture);
    if (Window::cudaTexture.textureResource) {
        errors |= CUDA::UnregisterTextureInterop(Window::cudaContext, Window::cudaTexture);
    }

    // Never explicitly delete a OpenGL resource that CUDA may still own.
    if (Window::glTexture.texture) {
        if (hasContext && 
            !Window::cudaTexture.textureResource &&
            !Window::cudaTexture.isMemMapped && 
            !Window::cudaTexture.surface) 
        {
            GL::ReleaseTexture(Window::glTexture);
        } else {
            errors |= ErrorCode::glTextureReleaseFailed;
        }
    }
    if (Window::glShader.shader) {
        if (hasContext) {
            GL::ReleaseShader(Window::glShader);
        } else {
            errors |= ErrorCode::glShaderReleaseFailed;
        }
    }
    if (Window::glVao.vao) {
        if (hasContext) {
            GL::ReleaseVao(Window::glVao);
        } else {
            errors |= ErrorCode::glVaoReleaseFailed;
        }
    }

    if (Window::window) {
        glfwDestroyWindow(Window::window);
    }
    glfwTerminate();

    // Terminal shutdown: Discards resource handles even when cleanup failed b/c no way to recover
    // If Cuda and/or OpenGL resources couldn't be released we can't retry anyways and should terminate
    Window::window = nullptr;
    Window::cudaTexture = {};
    Window::cudaContext = {};
    Window::glTexture   = {};
    Window::glShader    = {};
    Window::glVao       = {};
    return errors;
}

/*
    Utilities
*/

FrameView GetFrameView()
{
    return FrameView{Window::cudaTexture.surface, Window::glTexture.width, Window::glTexture.height};
}

void AttachInputCallbacks(KeyCallbackFn keyCallback, MouseButtonCallbackFn mouseButtonCallback)
{
    if (!Window::window) { return; }

    glfwSetKeyCallback(Window::window, keyCallback);
    glfwSetMouseButtonCallback(Window::window, mouseButtonCallback);
}

void PollEvents()
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
