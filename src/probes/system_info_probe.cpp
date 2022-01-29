#include "probes/system_info_probe.h"
#include "utils/utils.h"

#include <chrono>
#include <format>
#include <windows.h>
#include <winerror.h>
#include <winreg.h>

using namespace riptop;

SystemInfoProbe::SystemInfoProbe() { Update(); }

void SystemInfoProbe::Update()
{
    UpdateCpuCount();
    UpdateProcessorName();
    UpdateComputerName();
}

std::string SystemInfoProbe::GetUptime() const
{
    auto uptime = std::chrono::milliseconds(GetTickCount64());
    return std::format("{:%H:%M:%S}", std::chrono::duration_cast<std::chrono::seconds>(uptime));
}

void riptop::SystemInfoProbe::UpdateCpuCount()
{
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    cpu_core_count_ = system_info.dwNumberOfProcessors;
}

void riptop::SystemInfoProbe::UpdateProcessorName()
{
    static TCHAR cpu_name[256] {};
    HKEY         key;
    if (SUCCEEDED(RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0\\", &key)))
    {
        DWORD count = 256;
        if (SUCCEEDED(RegQueryValueEx(key, "ProcessorNameString", 0, 0, (LPBYTE)&cpu_name[0], &count)))
        {
            RegCloseKey(key);
        }
    }
    processor_name_ = cpu_name;
}

void riptop::SystemInfoProbe::UpdateComputerName()
{
    static TCHAR computer_name[16] {};
    DWORD        size = 16;
    GetComputerName(computer_name, &size);
    computer_name_ = computer_name;
}
