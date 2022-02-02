#include "ui/ui.h"
#include "ui/system_info_component.h"
#include "ui/process_list_component.h"
#include "services/system_data_service.h"
#include "services/acquisition_thread.h"

#include <ftxui/component/receiver.hpp>

#include <chrono>
#include <thread>

using namespace riptop;
using namespace ftxui;

void riptop::AcquisitionThread::operator()(Sender<std::vector<ProcessInfo>> process_sender,
                                           Sender<SystemInfoDynamicData>    system_info_sender,
                                           SystemDataService* system_data_service, Ui* main_ui)
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
