#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <string>

namespace riptop
{
    struct Process
    {
        HANDLE      handle;
        DWORD       id;
        std::string user_name;
        DWORD       base_priority;
        double      percent_processor_time;
        uint64_t    used_memory;
        DWORD       thread_count;
        uint64_t    up_time;
        std::string exe_name;
        DWORD       parent_pid;
        ULONGLONG   disk_operations_prev;
        ULONGLONG   disk_operations;
        DWORD       disk_usage;
        DWORD       tree_depth;
    };

    class ProcessListProbe
    {
      public:
        ProcessListProbe();
        bool                 UpdateProcessList(size_t update_interval_s);
        void                 SortProcessList();
        std::vector<Process> processes() { return processes_; };
        static const size_t  PROCESSES_INITIAL_COUNT {32};

      private:
        std::vector<Process> processes_;
    };

    void print_error(std::string msg);
} // namespace riptop
