#pragma once

#include "../include/CRRT.hpp"

#include <chrono>

/*
    Track Time and Expose it for the User
    -> Make sure UpdateTime() is called every frame such that the DeltaTime between frames is set
*/

namespace CRRT {

void StartTime()
{
    Time::LastUpdateTime = std::chrono::steady_clock::now();
    Time::DeltaTime = 0.0f;
}

void UpdateTime()
{
    auto timeNow = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<float, std::milli>(timeNow - Time::LastUpdateTime);
    Time::DeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(duration).count();
    Time::LastUpdateTime = timeNow;
}

};
