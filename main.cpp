#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "Hello, world!\n";
    for (int i = 0; i < 10; i++) {
        auto start = std::chrono::steady_clock::now();

        std::this_thread::sleep_for(std::chrono::milliseconds(166));

        auto end = std::chrono::steady_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;

        std::cout << "Elapsed Time " << elapsed_seconds.count() << " seconds\n";
    }

    return 0;
}