#pragma once

#include "probes/system_info_probe.h"
#include "probes/system_times_probe.h"
#include "probes/process_list_probe.h"
#include "probes/memory_usage_probe.h"


namespace riptop
{
    struct ProcessInfo;
    struct SystemInfoDynamicData;
    struct SystemInfoStaticData;

    class SystemDataService
    {
      public:
        SystemDataService() {}
        void Acquire(std::vector<ProcessInfo>& processes_infos, SystemInfoDynamicData& system_info_data);
        SystemInfoStaticData GetStaticData();

      private:
        SystemInfoProbe  system_info_;
        MemoryUsageProbe mem_info_;
        SystemTimesProbe system_times_;
        ProcessListProbe process_list_;
    };
} // namespace riptop