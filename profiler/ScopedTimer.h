#pragma once

#include <chrono>
#include "FrameStats.h"

using Clock = std::chrono::steady_clock;

class ScopedTimer {
    private:
    std::string sectionName;
    FrameStats& stats;
    Clock::time_point start;

public:
    ScopedTimer(const std::string& sectionName, FrameStats& stats);
    ~ScopedTimer();
};