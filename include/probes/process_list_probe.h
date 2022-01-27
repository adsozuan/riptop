#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <string>
#include <tlhelp32.h>

namespace riptop
{
    struct Process
    {
        HANDLE      handle {};
        DWORD       id {};
        std::string user_name {};
        DWORD       base_priority {};
        double      percent_processor_time {};
        uint64_t    used_memory {};
        DWORD       thread_count {};
        uint64_t    up_time {};
        std::string exe_name {};
        DWORD       parent_pid {};
        ULONGLONG   disk_operations_prev {};
        ULONGLONG   disk_operations {};
        DWORD       disk_usage {};
        DWORD       tree_depth {};

        Process() {};
        Process(const PROCESSENTRY32& process_entry);
        ~Process();

        void Update();
        void UpdateProcessMemoryUsage();
        void UpdateProcessUserName();
    };

    struct CompareProcess
    {
        DWORD id_;
        explicit CompareProcess(const DWORD& id) : id_(id) {}
        bool operator()(const DWORD& id) { return (id_ == id); }
    };

    class ProcessListProbe
    {
      public:
        ProcessListProbe();
        std::vector<Process> UpdateProcessList(size_t update_interval_s);
        void                 SortProcessList();

        //std::vector<Process> processes() { return processes_; };
        static const size_t  PROCESSES_INITIAL_COUNT {32};

        //std::vector<Process> processes_;
    };

    void print_error(std::string msg);
} // namespace riptop
