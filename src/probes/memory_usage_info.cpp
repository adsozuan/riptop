#include "../../include/probes/memory_usage_info.h"

MemoryUsageInfo::MemoryUsageInfo() {
  memory_info_.dwLength = sizeof(MEMORYSTATUSEX);
  Update();
};

void MemoryUsageInfo::Update() {
  GlobalMemoryStatusEx(&memory_info_);
  total_memory_ = to_mega_bytes(memory_info_.ullTotalPhys);
  used_memory_ =
      to_mega_bytes(memory_info_.ullTotalPhys - memory_info_.ullAvailPhys);
  used_memory_percentage_ = (double)used_memory_ / (double)total_memory_;

  total_page_memory_ = to_mega_bytes(memory_info_.ullTotalPageFile);
  used_page_memory_ =
      to_mega_bytes(memory_info_.ullTotalPageFile- memory_info_.ullAvailPageFile);
  used_page_memory_percentage_ = (double)used_page_memory_ / (double)total_page_memory_;
}
