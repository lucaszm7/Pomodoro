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

template<typename T>
std::vector<T> RandList(T start, T end, unsigned int size)
{
    std::vector<T> result;
    result.reserve(size);
    for(int i = 0; i < size; ++i)
    {
        result.emplace_back(rand<T>(start, end));
    }
    return &result;
}

template <typename T>
inline T rand(T start, T end)
{
    return (start + static_cast<T> (std::rand()) / (static_cast <T> (RAND_MAX / (end - start))));
}

