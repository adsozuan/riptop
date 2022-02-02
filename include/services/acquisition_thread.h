#include "ui/ui.h"
#include "services/system_data_service.h"

#include "ui/system_info_component.h"
#include "ui/process_list_component.h"

#include <ftxui/component/receiver.hpp>

using namespace ftxui;

namespace riptop
{
    class AcquisitionThread
    {
      public:
        void operator()(Sender<std::vector<ProcessInfo>> process_sender,
                        Sender<SystemInfoDynamicData> system_info_sender, SystemDataService* system_data_service,
                        Ui* main_ui);
    };

}
