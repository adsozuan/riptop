
#include "../../include/utils/formatter.h"

#include <cmath>
#include <cstdint>
#include <sstream>
#include <array>
#include <format>

std::string riptop::format_memory(int64_t memory)
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