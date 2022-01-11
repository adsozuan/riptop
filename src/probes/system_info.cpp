#include "../../include/probes/system_info.h"
#include "../../include/utils/formatter.h"

#include <chrono>
#include <format>
#include <windows.h>
#include <winerror.h>
#include <winreg.h>

using namespace riptop;

SystemInfo::SystemInfo()
{
    Update();
}

void SystemInfo::Update()
{
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    cpu_core_count_ = system_info.dwNumberOfProcessors;
    static TCHAR cpu_name[256];

    HKEY key;
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

std::string SystemInfo::GetUptime()
{
    auto uptime = std::chrono::milliseconds(GetTickCount64());
    return std::format("{:%H:%M:%S}", uptime);
}
