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
    ScopedTimer(const std::string& sectionName, FrameStats& stats) : sectionName(sectionName), stats(stats),
    start(Clock::now()) {}
    ~ScopedTimer() {
        auto end = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        stats.recordSection(sectionName, duration.count());
    }
};