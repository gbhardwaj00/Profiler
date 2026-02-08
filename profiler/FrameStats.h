#pragma once
#include <vector>
#include <cstdint>
#include <cstddef>

struct FrameSample
{
    int64_t workDuration;
    int64_t totalDuration;
};

class FrameStats
{
private:
    std::vector<FrameSample> frames;
    int bufferSize = 60;
    size_t writeIndex;
    size_t count;
    int64_t sumWorkUs;
    int64_t sumTotalUs;
    int64_t worstTotalUs;
    int worstFrameIndex;

public:
    void addSample(int frameIndex, int64_t workDuration, int64_t totalDuration);
    int64_t getAvgWorkUs() const;
    int64_t getAvgTotalUs() const;
    int64_t getWorstTotalUs() const;
    int getWorstFrameIndex() const;
    size_t getWindowSize() const;
    size_t getCapacity() const;
    void reset();
};