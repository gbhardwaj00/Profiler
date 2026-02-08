#include <iostream>
#include <chrono>
#include <thread>
#include <random>

static inline void simulateWork(std::chrono::microseconds workPeriod)
{
    volatile std::uint64_t dummy = 0;
    auto start = std::chrono::steady_clock::now();

    while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start) < workPeriod)
    {
        // cheap operations to keep CPU busy
        dummy = dummy * 1664525u + 1013904223u;
        dummy ^= (dummy >> 13);
    }
}

int main()
{
    // 1/60th of a second in microseconds
    // Represents the target frame time for a 60 FPS update loop
    std::uint64_t target_us = 16666;

    // Initialize random number generators
    std::mt19937 rng(12345); // fixed seed for reproducible runs
    std::uniform_int_distribution<int> normal_us(2000, 8000);
    std::uniform_int_distribution<int> spike_us(12000, 25000);
    std::bernoulli_distribution is_spike(0.05); // 5% chance

    for (int i = 0; i < 300; i++)
    {
        // Perform some work that takes time, simulating a frame update
        auto workStartAt = std::chrono::steady_clock::now();
        const int work_us = is_spike(rng) ? spike_us(rng) : normal_us(rng);
        simulateWork(std::chrono::microseconds(work_us));
        auto workEndAt = std::chrono::steady_clock::now();
        auto workDuration = std::chrono::duration_cast<std::chrono::microseconds>(workEndAt - workStartAt);

        // Calculate remaining time to sleep to maintain a consistent frame rate of 60 FPS
        auto remainingTime = std::chrono::microseconds(target_us) - workDuration;
        // Sleep for the remaining time if the work took less than the target frame time
        if (remainingTime > std::chrono::microseconds::zero())
        {
            while (std::chrono::steady_clock::now() - workStartAt < std::chrono::microseconds(target_us))
            {
                // intentionally empty: tight wait for precision
            }
        }

        auto frameEndAt = std::chrono::steady_clock::now();

        if (i % 30 == 0)
        {
            std::cout << "Work Duration " << workDuration.count() / 1000.0 << " ms\n";
            auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(frameEndAt - workStartAt);
            std::cout << "Elapsed Time " << elapsed_us.count() / 1000.0 << " ms\n";
        }
    }

    return 0;
}