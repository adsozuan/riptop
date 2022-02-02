#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include <string>
#include <tlhelp32.h>

namespace riptop
{
    class SystemTimesProbe;

    struct ProcessTimes
    {
        FILETIME CreationTime;
        FILETIME ExitTime;
        FILETIME KernelTime;
        FILETIME UserTime;
    };

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

        void                 Update();
        void                 AcquireMemoryUsage();
        void                 UpdateProcessUserName();
        riptop::ProcessTimes AcquireCpuUsage();
        void                 CalculateCpuUsage(uint64_t system_kernel_diff, uint64_t system_user_diff);
        void                 AcquireDiskUsage();
        void                 CalculateDiskUsage(size_t update_interval_ms);
        void                 AcquireUpTime();

        auto operator<=>(const Process& other) const = default;

      private:
        ProcessTimes process_times_ {};
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
        std::vector<Process> UpdateProcessList(size_t update_interval_ms, SystemTimesProbe& system_times_probe);
        size_t               running_process_count() { return running_process_count_; }

      private:
        static const size_t PROCESSES_INITIAL_COUNT {32};
        size_t              process_count_ {0};
        size_t              running_process_count_ {0};
    };

    void print_error(std::string msg);
} // namespace riptop
