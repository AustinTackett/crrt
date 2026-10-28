#include <cstdlib>
#include <iostream>

#include "DebugKernel.hpp"

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
        errors |= UpdateWindow(window, RenderDebugColors);

        // Display back buffer
        if (!errors) { 
            SwapBuffer(window);
        } 
    }

    // Host (CPU) must free device resources (GPU), so we still grab any errors
    errors |= ReleaseWindow(window);

    // Log all the errors for the user
    std::cout << GetErrorsString(errors);

    return errors ? EXIT_FAILURE : EXIT_SUCCESS;
}

