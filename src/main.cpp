#include "../include/ui/main_component.h"
#include "../include/probes/system_info_probe.h"
#include "../include/probes/system_times_probe.h"
#include "../include/probes/process_list_probe.h"
#include "../include/probes/memory_usage_probe.h"
#include "../include/utils/formatter.h"

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/receiver.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <chrono>
#include <thread>

using namespace riptop;
using namespace ftxui;

void SystemDataProducer(Sender<std::vector<ProcessInfo>> process_sender,
                        Sender<SystemInfoDynamicData> system_info_sender, SystemInfoProbe system_info,
                        MemoryUsageProbe mem_info, ScreenInteractive* screen)
{
    SystemTimesProbe         system_times;
    ProcessListProbe         process_list;

    while (true)
    {
        using namespace std::chrono_literals;
        std::vector<ProcessInfo> processes;

        system_info.Update();
        system_times.UpdateCpuUsage();
        mem_info.Update();
        auto incomings = process_list.UpdateProcessList(100);

        SystemInfoDynamicData system_info_data;
        system_info_data.cpu_usage                    = static_cast<float>(system_times.cpu_usage());
        system_info_data.memory_usage_percentage      = static_cast<float>(mem_info.used_memory_percentage());
        system_info_data.page_memory_usage_percentage = static_cast<float>(mem_info.used_page_memory_percentage());
        system_info_data.total_tasks_count            = incomings.size();
        system_info_data.up_time                      = system_info.GetUptime();

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
            processes.push_back(info);
        }

        system_info_sender->Send(system_info_data);
        process_sender->Send(processes);

        std::this_thread::sleep_for(500ms);
        screen->PostEvent(Event::Custom);
    }
}

int main(void)
{
    auto screen = ScreenInteractive::FitComponent();

    auto process_receiver = MakeReceiver<std::vector<ProcessInfo>>();
    auto process_sender   = process_receiver->MakeSender();

    auto system_data_receiver = MakeReceiver<SystemInfoDynamicData>();
    auto system_data_sender   = system_data_receiver->MakeSender();

    SystemInfoProbe      system_info;
    MemoryUsageProbe     mem_info;
    SystemInfoStaticData system_static_data;
    system_static_data.processor_name = system_info.processor_name();
    system_static_data.computer_name  = system_info.computer_name();
    system_static_data.total_memory   = mem_info.total_memory();

    auto component = std::make_shared<MainComponent>(std::move(process_receiver), system_static_data,
                                                     std::move(system_data_receiver));

    std::thread system_data_producer_thread(SystemDataProducer, std::move(process_sender),
                                            std::move(system_data_sender), std::move(system_info), std::move(mem_info),
                                            &screen);

    screen.Loop(component);
    system_data_producer_thread.join();

    return 0;
}
