
#include "utils/utils.h"

#include <cmath>
#include <cstdint>
#include <sstream>
#include <array>
#include <format>

std::string riptop::FormatMemory(int64_t memory)
{
    std::array<int64_t, 6>     dividers = {PETABYTE, TERABYTE, GIGABYTE, MEGABYTE, KILOBYTE, 1};
    std::array<std::string, 6> units    = {"PB", "TB", "GB", "MB", "KB", "B"};

    std::ostringstream oss;
    double             memory_decimal = static_cast<double>(memory);
    double             value {0.0};

    int index {0};
    for(auto& divider : dividers)
    {

        if (memory_decimal >= divider)
        {
            value = divider > 1 ? memory_decimal / static_cast<double>(divider) : memory_decimal;
            oss << std::format("{:.0f}{}", value, units[index]);
            break;
        }
        index++;
    }
    return oss.str();
}

uint64_t riptop::SubtractTimes(const FILETIME* a, const FILETIME* b)
{
    LARGE_INTEGER la {};
    LARGE_INTEGER lb {};

    la.LowPart  = a->dwLowDateTime;
    la.HighPart = b->dwHighDateTime;

    lb.LowPart  = b->dwLowDateTime;
    lb.HighPart = b->dwHighDateTime;

    return la.QuadPart - lb.QuadPart;
}