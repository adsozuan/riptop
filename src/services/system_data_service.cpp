#include "services/system_data_service.h"

#include "ui/system_info_component.h"
#include "ui/process_list_component.h"

using namespace riptop;

void SystemDataService::Acquire(std::vector<ProcessInfo>& processes_infos, SystemInfoDynamicData& system_info_data)
{
    system_info_.Update();
    system_times_.UpdateCpuUsage();
    mem_info_.Update();
    auto incomings = process_list_.UpdateProcessList(100, system_times_);

    system_info_data.cpu_usage                    = static_cast<float>(system_times_.cpu_usage());
    system_info_data.memory_usage_percentage      = static_cast<float>(mem_info_.used_memory_percentage());
    system_info_data.page_memory_usage_percentage = static_cast<float>(mem_info_.used_page_memory_percentage());
    system_info_data.total_tasks_count            = incomings.size();
    system_info_data.runnning_tasks_count         = process_list_.running_process_count();
    system_info_data.up_time                      = system_info_.GetUptime();

    for (auto& probe : incomings)
    {
        ProcessInfo info;
        info.id                     = probe.id;
        info.user_name              = probe.user_name;
        info.base_priority          = probe.base_priority;
        info.percent_processor_time = probe.percent_processor_time;
        info.used_memory            = probe.used_memory;
        info.thread_count           = probe.thread_count;
        info.disk_usage             = probe.disk_usage;
        info.exe_name               = probe.exe_name;
        processes_infos.push_back(info);
    }
}

SystemInfoStaticData SystemDataService::GetStaticData()
{
    SystemInfoStaticData system_static_data;
    system_static_data.processor_name = system_info_.processor_name();
    system_static_data.computer_name  = system_info_.computer_name();
    system_static_data.total_memory   = mem_info_.total_memory();
    return system_static_data;
}
