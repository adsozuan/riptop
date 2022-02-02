#include "ui/ui.h"
#include "services/system_data_service.h"

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/receiver.hpp>

#include <chrono>
#include <thread>

using namespace riptop;
using namespace ftxui;

void ProduceSystemData(Sender<std::vector<ProcessInfo>> process_sender,
                       Sender<SystemInfoDynamicData> system_info_sender, SystemDataService* system_data_service,
                       Ui* main_ui)
{

    while (true)
    {
        using namespace std::chrono_literals;
        std::vector<ProcessInfo> processes;

        SystemInfoDynamicData system_info_data;
        system_data_service->Acquire(processes, system_info_data);

        system_info_sender->Send(system_info_data);
        process_sender->Send(processes);

        std::this_thread::sleep_for(500ms);
        main_ui->PostEvent();
    }
}

int main(void)
{
    auto process_receiver = MakeReceiver<std::vector<ProcessInfo>>();
    auto process_sender   = process_receiver->MakeSender();

    auto system_data_receiver = MakeReceiver<SystemInfoDynamicData>();
    auto system_data_sender   = system_data_receiver->MakeSender();

    SystemDataService system_data_service;

    SystemInfoStaticData system_static_data = system_data_service.GetStaticData();

    Ui main_ui(system_static_data, std::move(system_data_receiver), std::move(process_receiver));


    std::jthread system_data_producer_thread(ProduceSystemData, std::move(process_sender),
                                             std::move(system_data_sender), &system_data_service, &main_ui);

    main_ui.Run();

    return 0;
}
