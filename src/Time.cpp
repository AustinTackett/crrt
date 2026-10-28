#include "Time.hpp"

namespace CRRT {

void StartTime()
{
    auto now = std::chrono::steady_clock::now();
    ClockContext::startClockTime = now;
    ClockContext::lastClockTime  = now;
    Time::CurrentTime = 0.0;
    Time::DeltaTime   = 0.0;
}

void UpdateTime()
{
    auto clockNow = std::chrono::steady_clock::now();
    Time::DeltaTime   = std::chrono::duration<double>(clockNow - ClockContext::lastClockTime).count();
    Time::CurrentTime = std::chrono::duration<double>(clockNow - ClockContext::startClockTime).count();
    ClockContext::lastClockTime = clockNow;
}

}
