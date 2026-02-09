#pragma once
#include <vector>
#include <cstdint>
#include <cstddef>

struct FrameSample
{
    int64_t workDuration;
    int64_t totalDuration;
    size_t frameIndex;
};

class FrameStats
{
private:
    std::vector<FrameSample> frames;
    constexpr static size_t bufferSize = 60;
    size_t writeIndex = 0;
    size_t count = 0;
    int64_t sumWorkUs = 0;
    int64_t sumTotalUs = 0;
    FrameSample worstFrameWindow = {};
    FrameSample worstFrameOverall = {};
    void recalculateWorstFrameWindow();

public:
    FrameStats();
    void addSample(int64_t workDuration, int64_t totalDuration, size_t currentFrameIndex);
    void reset();

    // Metrics
    int64_t getAvgWorkUs() const;
    int64_t getAvgTotalUs() const;
    FrameSample getWorstFrameOverall() const;
    FrameSample getWorstFrameWindow() const;
    double getFPS() const;

    // Buffer info
    size_t getWindowSize() const;
    size_t getCapacity() const;
    bool isFull() const;
};