
#include "../../include/utils/formatter.h"

#include <cmath>
#include <cstdint>
#include <sstream>

std::string riptop::format_memory(int64_t memory)
{
    std::ostringstream oss;
    double             memory_decimal = static_cast<double>(memory);
    double             value {0.0};

    if (memory < MEGABYTE)
    {
        value = std::ceil(memory_decimal / KILOBYTE);
        oss << value << "KB";
    }
    else if (memory < GIGABYTE)
    {
        value = std::ceil(memory_decimal / MEGABYTE);
        oss << value << "MB";
    }
    else if (memory < TERABYTE)
    {
        value = std::round(memory_decimal / GIGABYTE);
        oss << value << "GB";
    }
    else if (memory < PETABYTE)
    {
        value = std::round(memory_decimal / TERABYTE);
        oss << value << "TB";
    }
    else
    {
        value = std::round(memory_decimal / PETABYTE);
        oss << value << "PB";
    }

    return oss.str();
}