#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <random>
#include <cstdint>
#include "../profiler/FrameStats.h"
#include "../profiler/ScopedTimer.h"

using Clock = std::chrono::steady_clock;

// Simulates work by performing a busy-wait loop for the specified duration in microseconds
static inline void simulateWork(std::chrono::microseconds workPeriod)
{
    volatile std::uint64_t dummy = 0;
    const auto endTime = Clock::now() + workPeriod;

    int iter = 0;
    constexpr int CHECK_EVERY = 256; // check time every 256 iterations to avoid excessive overhead
    while (true)
    {
        // cheap operations to keep CPU busy
        dummy = dummy * 1664525u + 1013904223u;
        dummy ^= (dummy >> 13);
        if ((++iter % CHECK_EVERY) == 0)
        {
            if (Clock::now() >= endTime)
                break;
        }
    }
}

// Spins in a tight loop until the specified end time is reached
static inline void spinUntil(Clock::time_point endTime)
{
    while (Clock::now() < endTime)
    {
        // tight wait
    }
}

int main()
{   
    FrameStats stats;
    // ~60FPS
    const std::chrono::microseconds target_us(16666);

    // Initialize random number generators
    std::mt19937 rng(12345); // fixed seed for reproducible runs
    std::uniform_int_distribution<int> normal_us(2000, 8000);
    std::uniform_int_distribution<int> spike_us(12000, 25000);
    std::bernoulli_distribution is_spike(0.05); // 5% chance

    for (int i = 0; i < 300; i++)
    {
        const auto frameStartAt = Clock::now();
        const auto frameEndTarget = frameStartAt + target_us;

        // Input processing
        {
            ScopedTimer timer("Input", stats);
            int inputWorkUs = 200; // Fixed small amount
            simulateWork(std::chrono::microseconds(inputWorkUs));
        }
        // AI processing
        {
            ScopedTimer timer("AI", stats);
            int aiWorkUs = normal_us(rng) / 3;
            simulateWork(std::chrono::microseconds(aiWorkUs));
        }
        // Physics section 
        {
            ScopedTimer timer("Physics", stats);
            int physicsWorkUs = normal_us(rng) / 3;
            simulateWork(std::chrono::microseconds(physicsWorkUs));
        }
        // Render section (can have spikes)
        {
            ScopedTimer timer("Render", stats);
            int renderWorkUs = is_spike(rng) ? spike_us(rng) : normal_us(rng) / 3;
            simulateWork(std::chrono::microseconds(renderWorkUs));
        }
        
        const auto workEndAt = Clock::now();
        const auto workDuration = std::chrono::duration_cast<std::chrono::microseconds>(workEndAt - frameStartAt);

        // Spin if work finished early to maintain consistent frame timing
        if (workEndAt < frameEndTarget)
        {
            spinUntil(frameEndTarget);
        }

        // Log every 30 frames
        const auto frameEndAt = Clock::now();
        const auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEndAt - frameStartAt);
        stats.addSample(workDuration.count(), totalDuration.count(), i);
        if (i % 30 == 0 || i == 299)
        {
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "\n=== Frame " << i << " ===" << std::endl;
            std::cout << "Window Avg: " << stats.getAvgTotalUs() / 1000.0 << " ms" 
                      << " | FPS: " << stats.getFPS() 
                      << " | Window: " << stats.getWindowSize() << "/" << stats.getCapacity() << std::endl;
            
            // Worst frame in rolling window
            const auto& worstWindow = stats.getWorstFrameWindow();
            std::cout << "Worst (Window): Frame " << worstWindow.frameIndex 
                      << " = " << worstWindow.totalDuration / 1000.0 << " ms" << std::endl;
            if (!worstWindow.sections.empty()) {
                for (const auto& section : worstWindow.sections) {
                    std::cout << "  " << std::setw(10) << std::left << section.name 
                              << ": " << std::setw(7) << std::right << section.duration / 1000.0 << " ms" << std::endl;
                }
            }
            
            // Worst frame overall
            const auto& worstOverall = stats.getWorstFrameOverall();
            std::cout << "Worst (Overall): Frame " << worstOverall.frameIndex 
                      << " = " << worstOverall.totalDuration / 1000.0 << " ms" << std::endl;
            if (!worstOverall.sections.empty()) {
                for (const auto& section : worstOverall.sections) {
                    std::cout << "  " << std::setw(10) << std::left << section.name 
                              << ": " << std::setw(7) << std::right << section.duration / 1000.0 << " ms" << std::endl;
                }
            }
        }
    }

    return 0;
}