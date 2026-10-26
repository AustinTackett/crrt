#pragma once

#include <string>
#include <vector>

#include "Window.hpp"

namespace CRRT {

static std::string GetErrorString(const std::vector<WindowError>& errors) {
    std::string message;
    for (const WindowError& error : errors) {
        if (!message.empty()) message += '\n';
        message += GetWindowErrorString(error);
    }
    return message;
}

std::string ExecuteRenderLoop(WindowConfig windowConfig, DrawCallback drawCallback) {
    Window window;
    std::vector<WindowError> errors;

    // Return true if an error occured and add it to the list of errors
    const auto CheckAndLogError = [&errors](const WindowError& windowError) {
        if (CRRT::hasWindowError(windowError)) {
            errors.push_back(windowError);
            return true;
        }
        return false;
    };

    // Create Window
    if (CheckAndLogError(CRRT::CreateWindow(window, windowConfig))) {
        CheckAndLogError(CRRT::ReleaseWindow(window));
        CRRT::TerminateWindow();
        return GetErrorString(errors);
    }

    while (!CRRT::ShouldClose(window)) {
        // Draw User Logic
        if(CheckAndLogError(CRRT::UpdateWindow(window, drawCallback))) {
            break;
        }

        // Present Drawn Frame
        CRRT::SwapBuffer(window);
    }

    // Release resources.
    CheckAndLogError(CRRT::ReleaseWindow(window));
    CRRT::TerminateWindow();
    return GetErrorString(errors);
}

}