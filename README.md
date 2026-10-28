# README

TLDR: Lets you write cuda kernels and render it to the screen 

```Build Command

// Build Library
cmake --preset vs2022 -DFETCHCONTENT_QUIET=OFF
cmake --build --preset vs-debug --target SceneRayTracer

// Build Example
cmake --build --preset vs-debug --target debug-colors
.\build\vs2022\examples\debug-colors\Debug\debug-colors.exe

```

# Notes on coding style
Been writing more c99 and having lots of fun so wanted to practice writing more procedural programs 

# Example usage

void DrawCallback(CRRT::FrameView frame)
{
    // launch kernel and use WriteFrame() write pixels data
    
}

int main()
{
    // Window Configuration
    CRRT::Window window;
    CRRT::WindowConfig windowConfig;
    windowConfig.name       = "Example Window Name";
    windowConfig.initWidth  = windowConfig.resWidth = 1280;
    windowConfig.initHeight = windowConfig.resHeight = 720;

    // Loop untill error of requested to close
    CRRT::Errors errors = CreateWindow(window, windowConfig);
    while (!errors && !ShouldClose(window)) {  

        // Run callback and give access to draw to back buffer
        errors |= UpdateWindow(window, DrawCallback);

        // Display back buffer
        if (!errors) { 
            SwapBuffer(window);
        } 
    }

    // If the host can't free device resources, errors can still occur
    errors |= ReleaseWindow(window);

    // Log all the errors for the user
    std::cout << GetErrorsString(errors);

    return errors ? EXIT_FAILURE : EXIT_SUCCESS;
}

