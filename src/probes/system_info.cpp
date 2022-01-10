#include "../../include/probes/system_info.h"
#include "../../include/utils/formatter.h"

#include <chrono>
#include <format>
#include <windows.h>

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

    //  	HKEY Key;
    // if(SUCCEEDED(RegOpenKey(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0\\"), &Key))) {
    //	DWORD Count = 256;
    //	if(SUCCEEDED(RegQueryValueEx(Key, _T("ProcessorNameString"), 0, 0, (LPBYTE)&CPUName[0], &Count))) {
    //		RegCloseKey(Key);
    //	}
}

std::string SystemInfo::GetUptime()
{
    auto uptime = std::chrono::milliseconds(GetTickCount64());

    return std::format("{:%H:%M:%S}", uptime);
    return std::string();
}
