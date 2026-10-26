#include "DebugKernel.hpp"
#include <cstdlib>
#include <iostream>

int main()
{
    CRRT::WindowConfig config;
    config.name = "Debug Colors";
    config.initWidth = config.resWidth = 1280;
    config.initHeight = config.resHeight = 720;

    const auto error = CRRT::ExecuteRenderLoop(config, RenderDebugColors);
    if (!error.empty()) {
        std::cerr << error << '\n';
    }

    return error.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
}
