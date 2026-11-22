# CRRT: the "CUDA Rendering in Real Time" framework

Uses Cuda and OpenGL interop to create a framework where you can write you own custom rendering pipeline
in cuda and write it to the screen in real time.

Run these PowerShell commands from the repository root, using a Visual Studio generator.

# Building For Windows (Visual Studio)

Make sure you are in a Visual Studio environment for compiling Cuda or set the path variables manually
```powershell
# Visual Studio 2022 Example
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
```

```powershell
# Build library: .\build\vs\Release\CRRT.lib
cmake --preset vs
cmake --build --preset vs-release

# Build with example: .\build\debug-colors-vs\Release\debug-colors.exe
cmake -S examples/debug-colors -B build/debug-colors-vs
cmake --build build/debug-colors-vs --config Release
```

# Building For Other Platforms
I only have cuda comptatible devices on my windows computer, so I have not tested the application outside of windows.

# Note on Cuda Devices
At the moment I am fetching the first listed CUDA compatible device automatically. I will eventually swap that to be either an arguement to the create window function or automate it by looking at each cuda device properties.

# Usage

## Example Cuda Kernel 
```Cpp
__global__ void DrawToPixels(CRRT::FrameView frame, float color)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= frame.width || y >= frame.height) return;

    float4 rgba = make_float4(color, color, color, 1.0f);

    // All frame writing/read functions in "FrameOperations.cu"
    CRRT::WriteFrameRgb(frame, x, y, rgba);
}
```

OpenGL and Cuda Interop can result in unrecoverable (sticky) hardware level errors. So if there is an error its generally advisable to terminate. When the Crrt public functions return an error, the resources are also unmapped/released to avoid deadlocking. 

## Using Cuda Kernel to Draw to Window in Real Time
```Cpp
#include <cstdlib>
#include <iostream>

#include <Crrt.hpp> // Public header for Crrt

void DrawCallback(CRRT::FrameView frame)
{
    // (Anything you want to execute)...

    // Cuda kernel to write to screen
    DrawToPixels<<<gridDim, blockDim>>>(frame, color);
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
## Examples

Generated from, "examples/debug-colors": displays slowly changing colors to illustrate the concept of cuda writing color data to window in real time.

![DebugColorsExample](./examples/images/debug-colors.png)

## Notes on coding style
For other personal projects, I have been writing lots of c99, so I wanted to write a fun personal project inspired by a procedural style like you would see in older c-api's. Although its hard to ignore c++ features like pasing by reference, the Standard Template Library, and namespaces.