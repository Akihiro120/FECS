#pragma once

#include <chrono>
#include <cstdio>

template <typename T>
inline void DoNotOptimize(const T& value)
{
    asm volatile("" : : "g"(value) : "memory");
}

#pragma once

#include <chrono>
#include <cstdio>

class Benchmark
{
public:
    explicit Benchmark(const char* label = nullptr)
        : label_(label), start_(std::chrono::steady_clock::now()) {}

    ~Benchmark()
    {
        auto end = std::chrono::steady_clock::now();
        auto duration_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end - start_).count();
        if (label_)
        {
            std::printf("%s: %.10f ms\n", label_, duration_ms);
        }
        else
        {
            std::printf("Duration: %.3f ms\n", duration_ms);
        }
    }

private:
    const char* label_;
    std::chrono::steady_clock::time_point start_;
};
