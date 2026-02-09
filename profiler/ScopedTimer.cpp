#include "ScopedTimer.h"

ScopedTimer::ScopedTimer(const std::string& sectionName, FrameStats& stats) : sectionName(sectionName), stats(stats),
    start(Clock::now()) {}

ScopedTimer::~ScopedTimer() {
    auto end = Clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    stats.recordSection(sectionName, duration.count());
}