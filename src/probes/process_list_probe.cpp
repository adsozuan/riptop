#include "probes/process_list_probe.h"
#include "probes/system_times_probe.h"
#include "utils/utils.h"

#include <algorithm>
#include <chrono>
#include <format>
#include <iostream>
#include <sstream>
#include <Psapi.h>
#include <tchar.h>
#include <thread>
#include <stdexcept>
#include <wil/resource.h>
#include <wil/token_helpers.h>

riptop::ProcessListProbe::ProcessListProbe() {}

std::vector<riptop::Process> riptop::ProcessListProbe::UpdateProcessList(size_t            update_interval_ms,
                                                                         SystemTimesProbe& system_times_probe)
{
    HANDLE         process_snap {};
    PROCESSENTRY32 process_entry {};
    DWORD          dwPriorityClass;

    // Take a snapshot of all processes in the system.
    process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (process_snap == INVALID_HANDLE_VALUE)
    {
        print_error("CreateToolhelp32Snapshot (of processes) failed.");
        throw std::runtime_error {"CreateToolhelp32Snapshot of process failed."};
    }

    // Set the size of the structure before using it.
    process_entry.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(process_snap, &process_entry))
    {
        print_error(TEXT("Process32First failed.")); // show cause of failure
        CloseHandle(process_snap);                   // clean the snapshot object
        throw std::runtime_error {"Process32First failed at snapshot."};
    }

    std::vector<Process> processes;

    // Now walk the snapshot of processes, and
    int index {0};
    do
    {
        {
            Process process(process_entry);
            dwPriorityClass = 0;
            if (process.handle)
            {
                processes.push_back(process);
                dwPriorityClass = GetPriorityClass(process.handle);
                index++;
            }
        }

    } while (Process32Next(process_snap, &process_entry));

    CloseHandle(process_snap);

    auto previous_system_times = system_times_probe.AcquireSystemTimes();

    for (auto& process : processes)
    {
        process.AcquireCpuUsage();
        process.AcquireDiskUsage();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(update_interval_ms));

    auto system_times       = system_times_probe.AcquireSystemTimes();
    auto system_kernel_diff = SubtractTimes(&system_times.kernel_time, &previous_system_times.kernel_time);
    auto system_user_diff   = SubtractTimes(&system_times.user_time, &previous_system_times.user_time);

    running_process_count_ = 0;

    for (auto& process : processes)
    {
        if (process.handle)
        {
            process.CalculateCpuUsage(system_kernel_diff, system_user_diff);
            if (process.percent_processor_time >= 0.01)
            {
                running_process_count_++;
            }
            process.AcquireUpTime();
            process.CalculateDiskUsage(update_interval_ms);
            CloseHandle(process.handle);
            process.handle = 0;
        }
    }

    process_count_ = processes.size();

    return processes;
}

riptop::Process::Process(const PROCESSENTRY32& process_entry)
{
    id            = process_entry.th32ProcessID;
    thread_count  = process_entry.cntThreads;
    base_priority = process_entry.pcPriClassBase;
    parent_pid    = process_entry.th32ParentProcessID;
    exe_name      = std::string(process_entry.szExeFile);
    user_name     = std::string("SYSTEM");
    handle        = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
    if (handle)
    {
        AcquireMemoryUsage();
        UpdateProcessUserName();
    }
}

riptop::Process::~Process() {}

void riptop::Process::AcquireMemoryUsage()
{
    PROCESS_MEMORY_COUNTERS proc_mem_counters;

    if (GetProcessMemoryInfo(handle, &proc_mem_counters, sizeof(proc_mem_counters)))
    {
        used_memory = static_cast<uint64_t>(proc_mem_counters.WorkingSetSize);
    }
}

void riptop::Process::UpdateProcessUserName()
{
    wil::unique_handle process_token_handle;

    if (OpenProcessToken(handle, TOKEN_READ, &process_token_handle))
    {
        DWORD ReturnLength;

        GetTokenInformation(process_token_handle.get(), TokenUser, 0, 0, &ReturnLength);

        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {

            auto token_user = wil::get_token_information<TOKEN_USER>();

            if (GetTokenInformation(process_token_handle.get(), TokenUser, token_user.get(), ReturnLength,
                                    &ReturnLength))
            {
                SID_NAME_USE NameUse;
                DWORD        NameLength = 256;
                TCHAR        DomainName[MAX_PATH];
                DWORD        DomainLength    = MAX_PATH;
                std::string  user_name_local = "";
                LPSTR        user_name_out   = _strdup(user_name_local.c_str());

                LookupAccountSid(0, token_user.get()->User.Sid, user_name_out, &NameLength, DomainName, &DomainLength,
                                 &NameUse);

                user_name = user_name_out;
            }
        }
    }
};

riptop::ProcessTimes riptop::Process::AcquireCpuUsage()
{
    if (handle)
    {
        GetProcessTimes(handle, &process_times_.CreationTime, &process_times_.ExitTime, &process_times_.KernelTime,
                        &process_times_.UserTime);
    }
    return process_times_;
}

void riptop::Process::CalculateCpuUsage(uint64_t system_kernel_diff, uint64_t system_user_diff)
{
    auto     previous_times   = process_times_;
    auto     current_times    = AcquireCpuUsage();
    uint64_t proc_kernel_diff = SubtractTimes(&current_times.KernelTime, &previous_times.KernelTime);
    uint64_t proc_user_diff   = SubtractTimes(&current_times.UserTime, &previous_times.UserTime);

    uint64_t total_system = system_kernel_diff + system_user_diff;
    uint64_t total_proc   = proc_kernel_diff + proc_user_diff;

    if (total_system > 0)
    {
        percent_processor_time =
            static_cast<double>(((100.0 * static_cast<double>(total_proc) / static_cast<double>(total_system))));
    }
}

void riptop::Process::AcquireDiskUsage()
{
    if (handle)
    {
        IO_COUNTERS io_counters;
        if (GetProcessIoCounters(handle, &io_counters))
        {
            disk_operations_prev = io_counters.ReadOperationCount + io_counters.WriteTransferCount;
        }
    }
}

void riptop::Process::CalculateDiskUsage(size_t update_interval_ms)
{
    if (handle)
    {
        IO_COUNTERS io_counters;
        if (GetProcessIoCounters(handle, &io_counters))
        {
            disk_operations = io_counters.ReadOperationCount + io_counters.WriteTransferCount;
            disk_usage = static_cast<DWORD>((disk_operations - disk_operations_prev) * (1000) / update_interval_ms);
        }
    }
}

void riptop::Process::AcquireUpTime()
{
    FILETIME system_time;
    GetSystemTimeAsFileTime(&system_time);

    up_time = SubtractTimes(&system_time, &process_times_.CreationTime) / 10000;
}

void riptop::print_error(std::string msg)
{
    DWORD  eNum;
    TCHAR  sysMsg[256];
    TCHAR* p;

    eNum = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, eNum,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                  sysMsg, 256, NULL);

    // Trim the end of the line and terminate it with a null
    p = sysMsg;
    while ((*p > 31) || (*p == 9))
        ++p;
    do
    {
        *p-- = 0;
    } while ((p >= sysMsg) && ((*p == '.') || (*p < 33)));

    // Display the message
    std::cout << std::format("\n  WARNING: %s failed with error %d (%s)", msg, eNum, sysMsg) << '\n';
}
