#include "..\..\include\probes\system_times.h"

#include <algorithm>
#include <thread>

void SystemTimes::UpdateCpuUsage()
{
    using namespace std::chrono_literals;

    GetSystemTimes(&previous_.idle_time, &previous_.kernel_time, &previous_.user_time);

    std::this_thread::sleep_for(500ms); // not nice

    GetSystemTimes(&current_.idle_time, &current_.kernel_time, &current_.user_time);

    uint64_t kernel_diff = SubtractTimes(&current_.kernel_time, &previous_.kernel_time);
    uint64_t user_diff   = SubtractTimes(&current_.user_time, &previous_.user_time);
    uint64_t idle_diff   = SubtractTimes(&current_.idle_time, &previous_.idle_time);

    uint64_t system_time = kernel_diff + user_diff;
    if (system_time > 0)
    {
        double percentage = static_cast<double>(system_time - idle_diff) / static_cast<double>(system_time);
        cpu_usage_        = std::min<double>(percentage, 1.0);
    }
}

uint64_t SystemTimes::SubtractTimes(const FILETIME* a, const FILETIME* b)
{
    LARGE_INTEGER la {};
    LARGE_INTEGER lb {};

    la.LowPart  = a->dwLowDateTime;
    la.HighPart = b->dwHighDateTime;

    lb.LowPart  = b->dwLowDateTime;
    lb.HighPart = b->dwHighDateTime;

    return la.QuadPart - lb.QuadPart;
}
