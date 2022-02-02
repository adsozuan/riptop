#include "ui/ui.h"
#include "services/system_data_service.h"
#include "services/acquisition_thread.h"

#include <thread>

using namespace riptop;
using namespace ftxui;


/******************************************************************************************************************

main -->-->-----UI thread ----------->------------------- Receives probes data -->-- Render -->-- Handle Inputs ---
        \                                                        |     |
         \                                                       ^     ^
          \                                  system_info_channel |     | processes_channel
           \                                                     ^     ^  
            \                                                     \   /
             `--Acquisition thread -->-- Acquire all probes -->-- Send probes data to UI --------------------------

*******************************************************************************************************************/
int main(void)
{
    SystemDataService system_data_service;
    SystemInfoStaticData system_static_data = system_data_service.GetStaticData();

    SystemInfoDynChannel system_info_channel;
    ProcessesChannel processes_channel;


    Ui main_ui(system_static_data, std::move(system_info_channel.rx), std::move(processes_channel.rx));

    std::jthread acquisition_thread(AcquisitionThread(),std::move(processes_channel.tx),
                                             std::move(system_info_channel.tx), &system_data_service, &main_ui);


    main_ui.Run();

    return 0;
}
