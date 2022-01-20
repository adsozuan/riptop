#include "../../include/probes/memory_usage_probe.h"

using namespace riptop;

MemoryUsageProbe::MemoryUsageProbe()
{
    memory_info_.dwLength = sizeof(MEMORYSTATUSEX);
    Update();
};

void MemoryUsageProbe::Update()
{
    GlobalMemoryStatusEx(&memory_info_);
    total_memory_           = (memory_info_.ullTotalPhys);
    used_memory_            = (memory_info_.ullTotalPhys - memory_info_.ullAvailPhys);
    used_memory_percentage_ = static_cast<double>(used_memory_) / static_cast<double>(total_memory_);

    total_page_memory_           = (memory_info_.ullTotalPageFile);
    used_page_memory_            = (memory_info_.ullTotalPageFile - memory_info_.ullAvailPageFile);
    used_page_memory_percentage_ = static_cast<double>(used_page_memory_) / static_cast<double>(total_page_memory_);
}
