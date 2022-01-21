#pragma once

#include <cstdint>
#include <windows.h>

namespace riptop
{
    struct TimesData
    {
        FILETIME idle_time ;
        FILETIME kernel_time ;
        FILETIME user_time ;
    };

    class SystemTimesProbe
    {
      public:
        void   UpdateCpuUsage();
        double cpu_usage() const { return cpu_usage_; }

      private:
        uint64_t SubtractTimes(const FILETIME* a, const FILETIME* b);

      private:
        double    cpu_usage_ {0.0};
        TimesData current_  {};
        TimesData previous_ {};
    };
} // namespace riptop
