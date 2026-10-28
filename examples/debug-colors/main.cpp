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

    // Loop until error of requested to close
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

