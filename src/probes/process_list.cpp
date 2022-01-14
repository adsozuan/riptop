#include "..\..\include\probes\process_list.h"

#include <format>
#include <iostream>
#include <tchar.h>
#include <tlhelp32.h>

riptop::ProcessList::ProcessList()
{
    processes_.reserve(300);
}

void riptop::ProcessList::SortProcessList()
{
}

bool riptop::ProcessList::UpdateProcessList(size_t update_interval_s)
{
    HANDLE         process_snap;
    PROCESSENTRY32 process_entry {};
    DWORD          dwPriorityClass;

    // Take a snapshot of all processes in the system.
    process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (process_snap == INVALID_HANDLE_VALUE)
    {
        print_error("CreateToolhelp32Snapshot (of processes) failed.");
        return false;
    }

    // Set the size of the structure before using it.
    process_entry.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(process_snap, &process_entry))
    {
        print_error(TEXT("Process32First failed.")); // show cause of failure
        CloseHandle(process_snap);           // clean the snapshot object
        return (FALSE);
    }

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    do
    {
        Process process       = {0};
        process.id            = process_entry.th32ProcessID;
        process.thread_count  = process_entry.cntThreads;
        process.base_priority = process_entry.pcPriClassBase;
        process.parent_pid    = process_entry.th32ParentProcessID;
        process.exe_name      = std::string(process_entry.szExeFile);
        process.user_name     = std::string("SYSTEM");

        // Retrieve the priority class.
        dwPriorityClass = 0;
        process.handle  = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID);
        {
            if (processes_.size() < PROCESS_MAX_NUMBER)
                processes_.push_back(process);

            dwPriorityClass = GetPriorityClass(process.handle);
            CloseHandle(process.handle);
        }

    } while (Process32Next(process_snap, &process_entry));

    CloseHandle(process_snap);
    return (TRUE);
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
