#include "FrameStats.h"
#include <algorithm>

FrameStats::FrameStats() {
    frames.resize(bufferSize);
}

void FrameStats::addSample(int64_t workDuration, int64_t totalDuration, size_t currentFrameIndex) {
    // Write index is the current frame index modulo the buffer size
    writeIndex = currentFrameIndex % bufferSize;
    if (isFull()) {
        sumWorkUs -= frames[writeIndex].workDuration;
        sumTotalUs -= frames[writeIndex].totalDuration;
    }
    frames[writeIndex] = FrameSample{workDuration, totalDuration, currentFrameIndex};
    frames[writeIndex].sections = std::move(currentFrameSections);  // Move sections, avoid copy
    currentFrameSections.clear();  // Clear for next frame
    sumWorkUs += workDuration;
    sumTotalUs += totalDuration;

    // Compare to overall worst frame (copy from frames[writeIndex] to include sections)
    if (totalDuration > worstFrameOverall.totalDuration){
        worstFrameOverall = frames[writeIndex];
    }

    // Compare to worst frame in window (copy from frames[writeIndex] to include sections)
    if (totalDuration > worstFrameWindow.totalDuration){
        worstFrameWindow = frames[writeIndex];
    }

    // Recalculate worst frame in window it is not a part of the window anymore
    if (currentFrameIndex - worstFrameWindow.frameIndex > bufferSize - 1){
        recalculateWorstFrameWindow();
    }

    count = std::min(count + 1, bufferSize);
}

// Recalculate the worst frame in window
void FrameStats::recalculateWorstFrameWindow() {
    worstFrameWindow = {};
    for (size_t i = 0; i < bufferSize; i++) {
        if (frames[i].totalDuration > worstFrameWindow.totalDuration) {
            worstFrameWindow = frames[i];
        }
    }
}

void FrameStats::reset() {
    frames.assign(bufferSize, {});
    writeIndex = 0;
    count = 0;
    sumWorkUs = 0;
    sumTotalUs = 0;
    worstFrameWindow = {};
    worstFrameOverall = {};
    currentFrameSections.clear();
}

void FrameStats::recordSection(const std::string& name, int64_t duration) {
    currentFrameSections.push_back({name, duration});
}

int64_t FrameStats::getAvgWorkUs() const {
    return count > 0 ? sumWorkUs / count : 0;
}

int64_t FrameStats::getAvgTotalUs() const {
    return count > 0 ? sumTotalUs / count : 0;
}

FrameSample FrameStats::getWorstFrameOverall() const {
    return worstFrameOverall;
}

FrameSample FrameStats::getWorstFrameWindow() const {
    return worstFrameWindow;
}

double FrameStats::getFPS() const {
    int64_t avg = getAvgTotalUs();
    return avg > 0 ? 1000000.0 / avg : 0.0;
}

size_t FrameStats::getWindowSize() const {
    return count;
}

size_t FrameStats::getCapacity() const {
    return frames.size();
}

bool FrameStats::isFull() const {
    return count == frames.size();
}