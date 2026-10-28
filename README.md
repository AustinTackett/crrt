# CRRT: the "CUDA Rendering in Real Time" framework

Uses Cuda and OpenGL interop to create a framework where you can write you own custom rendering pipeline
in cuda and write it to the screen in real time.

Run these PowerShell commands from the repository root, using a Visual Studio generator.

# Building For Windows (Visual Studio)
```powershell
# Build library: .\build\vs\Release\CRRT.lib
cmake --preset vs
cmake --build --preset vs-release

# Build example: .\build\debug-colors-vs\Release\debug-colors.exe (builds library too)
cmake -S examples/debug-colors -B build/debug-colors-vs
cmake --build build/debug-colors-vs --config Release
```

# Notes on coding style
For othe personal project, I have been writing lots of c99, so I wanted to write a project using more
of that procedural style of programming.

# Example usage

```Cpp
#include <cstdlib>
#include <iostream>

/*
    OpenGL and Cuda Interop can result in unrecoverable errors and should terminate the program
*/

void DrawCallback(CRRT::FrameView frame)
{
    // Launch kernel containing WriteFrame(x, y, color) to write to individual pixels   
}

int main()
{
    // Window Configuration
    CRRT::WindowConfig windowConfig;
    windowConfig.name       = "Example Window Name";
    windowConfig.initWidth  = windowConfig.resWidth  = 1920;
    windowConfig.initHeight = windowConfig.resHeight = 1080;

    // Create GLFW window
    CRRT::Errors errors = CRRT::CreateWindow(windowConfig);
    if (errors) {
        std::cerr << GetErrorsString(errors);
        return EXIT_FAILURE;
    }

    while (!CRRT::ShouldClose()) {  

        // Run callback and give access to draw to back buffer
        errors |= CRRT::UpdateWindow(RenderDebugColors);
        if (errors) {
            std::cerr << GetErrorsString(errors);
            return EXIT_FAILURE;
        }

        CRRT::SwapBuffer(); 
    }

    // If the host can't free device resources, errors can still occur
    errors |= CRRT::ReleaseWindow();
    std::cout << GetErrorsString(errors);
    return errors ? EXIT_FAILURE : EXIT_SUCCESS;
}
```
