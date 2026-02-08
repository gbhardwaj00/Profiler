#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <cstdint>

using Clock = std::chrono::steady_clock;

// Simulates work by performing a busy-wait loop for the specified duration in microseconds
static inline void simulateWork(std::chrono::microseconds workPeriod)
{
    volatile std::uint64_t dummy = 0;
    const auto endTime = Clock::now() + workPeriod;

    int iter = 0;
    constexpr int CHECK_EVERY = 256; // check time every 1000 iterations to avoid excessive overhead
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

// Spins in a tight loop until the specified end time is reached, providing more precise timing than sleep-based waits
static inline void spinUntil(Clock::time_point endTime)
{
    while (Clock::now() < endTime)
    {
        // tight wait
    }
}

int main()
{
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

        const int plannedWorkUs = is_spike(rng) ? spike_us(rng) : normal_us(rng);
        simulateWork(std::chrono::microseconds(plannedWorkUs));

        const auto workEndAt = Clock::now();
        const auto workDuration = std::chrono::duration_cast<std::chrono::microseconds>(workEndAt - frameStartAt);

        if (workEndAt < frameEndTarget)
        {
            spinUntil(frameEndTarget);
        }

        // Log the work duration and total elapsed time every 30 iterations
        const auto frameEndAt = Clock::now();
        const auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEndAt - frameStartAt);

        if (i % 30 == 0)
        {
            std::cout
                << "Planned " << plannedWorkUs / 1000.0 << " ms | "
                << "Work " << workDuration.count() / 1000.0 << " ms | "
                << "Total " << totalDuration.count() / 1000.0 << " ms\n";
        }
    }

    return 0;
}