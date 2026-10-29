#include <cstdlib>
#include <iostream>

#include "DebugKernel.hpp"

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

    // Draw Callback: This runs every frame and should contain your cuda kernel
    auto drawBackCall = [&] (CRRT::FrameView frame) {
        RenderDebugColors(frame);
        // ...
    };

    while (!CRRT::ShouldClose()) {  

        // Run callback and give access to draw to back buffer
        errors |= CRRT::UpdateWindow(drawBackCall);
        if (errors) {
            std::cerr << GetErrorsString(errors);
            return EXIT_FAILURE;
        }

        // Display what was drawn
        CRRT::SwapBuffer(); 
    }

    // If the host can't free device resources, errors can still occur
    errors |= CRRT::ReleaseWindow();
    std::cout << GetErrorsString(errors);
    return errors ? EXIT_FAILURE : EXIT_SUCCESS;
}

