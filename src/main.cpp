#include "ui/ui.h"
#include "services/system_data_service.h"
#include "services/acquisition_thread.h"

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/receiver.hpp>

#include <chrono>
#include <thread>

using namespace riptop;
using namespace ftxui;

int main(void)
{
    SystemDataService system_data_service;
    SystemInfoStaticData system_static_data = system_data_service.GetStaticData();

    auto process_receiver = MakeReceiver<std::vector<ProcessInfo>>();
    auto process_sender   = process_receiver->MakeSender();

    auto system_data_receiver = MakeReceiver<SystemInfoDynamicData>();
    auto system_data_sender   = system_data_receiver->MakeSender();


    Ui main_ui(system_static_data, std::move(system_data_receiver), std::move(process_receiver));


    std::jthread system_data_producer_thread(AcquisitionThread(),std::move(process_sender),
                                             std::move(system_data_sender), &system_data_service, &main_ui);

    main_ui.Run();

    return 0;
}
