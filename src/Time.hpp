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
    Time::Clock = std::chrono::steady_clock::now();
    Time::CurrentTime = 0.0f;
    Time::DeltaTime = 0.0f;
}

void UpdateTime()
{
    auto clockNow = std::chrono::steady_clock::now();
    
    std::chrono::duration<float> deltaDuration = clockNow - Time::Clock;
    Time::DeltaTime = deltaDuration.count();
    Time::CurrentTime += Time::DeltaTime;

    Time::Clock = clockNow;
    
}

};
