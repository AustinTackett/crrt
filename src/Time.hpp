#pragma once

#include <chrono>

namespace CRRT {

/*

From: include/Crrt.hpp

struct Time
{
    inline static double CurrentTime;
    inline static double DeltaTime = 0.0f;
};

*/

struct ClockContext
{
    inline static std::chrono::steady_clock::time_point startClockTime;
    inline static std::chrono::steady_clock::time_point lastClockTime;
};

void StartTime();

void UpdateTime();

}

