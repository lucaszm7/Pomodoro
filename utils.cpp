#pragma once

#include <chrono>
#include <vector>

struct Timer
{
    std::chrono::steady_clock::time_point start;
    std::chrono::duration<double> duration;

    Timer(std::string n)
    {
        start = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        // duration = std::chrono::high_resolution_clock::now() - start;
        // std::cout << "\nTook: " << duration.count() * 1000.0 << "ms\n";
    }

    double now()
    {
        duration = std::chrono::high_resolution_clock::now() - start;
        return duration.count();
    }

    double nowMs()
    {
        duration = std::chrono::high_resolution_clock::now() - start;
        return duration.count() * 1000.0;
    }

    void reset()
    {
        start = std::chrono::high_resolution_clock::now();
    }
};

std::vector<float> RandList(float start, float end, unsigned int size)
{
    std::vector<float> result;
    result.reserve(size);
    for(int i = 0; i < size; ++i)
    {
        result.emplace_back(rand(start, end));
    }
}

inline float rand(float start, float end)
{
    return (start + static_cast<float> (std::rand()) / (static_cast <float> (RAND_MAX / (end - start))));
}

