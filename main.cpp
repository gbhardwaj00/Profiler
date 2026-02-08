#include <iostream>
#include <chrono>
#include <thread>

static inline void simualteWork(std::chrono::microseconds workPeriod)
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

    for (int i = 0; i < 300; i++)
    {
        // Perform some work that takes time, simulating a frame update
        auto workStartAt = std::chrono::steady_clock::now();
        std::chrono::microseconds workFor(5000); // Simulate work that takes approximately 5 ms
        simualteWork(workFor);
        auto workEndAt = std::chrono::steady_clock::now();
        auto workDuration = std::chrono::duration_cast<std::chrono::microseconds>(workEndAt - workStartAt);

        // Calculate remaining time to sleep to maintain a consistent frame rate of 60 FPS
        auto remainingTime = std::chrono::microseconds(target_us) - workDuration;
        if (remainingTime > std::chrono::microseconds::zero())
        {
            constexpr auto spin_budget = std::chrono::microseconds(10000);
            if (remainingTime > spin_budget)
            {
                // Sleep for the majority of the remaining time, leaving a small budget for spinning
                std::this_thread::sleep_for(remainingTime - spin_budget);
            }
            // Spin the last bit (fine)
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