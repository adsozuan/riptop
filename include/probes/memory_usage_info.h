#pragma once
#include <windows.h>
#include <cstdint>

constexpr auto to_mega_bytes(int64_t x) {
  return x / 1048576;
}


class MemoryUsageInfo {
 public:
  MemoryUsageInfo();
  void  Update();

  int64_t total_memory() { return total_memory_; }
  int64_t total_memory_GB() { return total_memory_ / 1000; }
  int64_t used_memory() { return used_memory_; }
  double used_memory_percentage() { return used_memory_percentage_; }

  int64_t total_page_memory() { return total_page_memory_; }
  int64_t total_page_memory_GB() { return total_page_memory_ / 1000; }
  int64_t used_page_memory() { return used_page_memory_; }
  double used_page_memory_percentage() { return used_page_memory_percentage_; }

 private:
  MEMORYSTATUSEX memory_info_;
  int64_t total_memory_{0};
  int64_t used_memory_{0};
  double used_memory_percentage_{0};
  int64_t total_page_memory_{0};
  int64_t used_page_memory_{0};
  double used_page_memory_percentage_{0};
};
