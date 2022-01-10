
#include "../../include/utils/formatter.h"

#include <cstdint>
#include <sstream>

const int64_t BYTE     = 1;
const int64_t KILOBYTE = 1024;
const int64_t MEGABYTE = KILOBYTE * KILOBYTE;
const int64_t GIGABYTE = MEGABYTE * KILOBYTE;
const int64_t TERABYTE = 1024ULL * GIGABYTE;

std::string riptop::format_memory(int64_t memory)
{
    std::ostringstream oss;
    int64_t            value;

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