
#include "../../include/utils/formatter.h"

#include <sstream>
#include <cstdint>


const unsigned long BYTE     = 1;
const unsigned long KILOBYTE = 1024;
const unsigned long MEGABYTE = KILOBYTE * KILOBYTE;
const unsigned long GIGABYTE = MEGABYTE * KILOBYTE;
const unsigned long long TERABYTE = 1024ULL * GIGABYTE;

std::string riptop::format_memory(int64_t memory)
{
    std::ostringstream oss;
    double             value;

    if (memory < MEGABYTE)
    {
        value = memory / KILOBYTE;
        oss << value << "KB";
    }
    else if (memory < GIGABYTE)
    {
        value = memory / MEGABYTE;
        oss << value << "MB";
    }
    else if (memory < TERABYTE)
    {
        value = memory / GIGABYTE;
        oss << value << "GB";
    }
    else
    {
        value = memory / TERABYTE;
        oss << value << "TB";
    }

    return oss.str();
}