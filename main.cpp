#include <iostream>
#include <chrono>
#include <thread>

int main()
{
    std::cout << "Hello, world!\n";
    for (int i = 0; i < 300; i++)
    {
        auto start = std::chrono::steady_clock::now();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        auto end = std::chrono::steady_clock::now();

        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Elapsed Time " << elapsed_ms.count() << " ms\n";
    }

    return 0;
}