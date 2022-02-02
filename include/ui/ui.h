#pragma once

#include "ui/main_component.h"

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/receiver.hpp>

namespace riptop
{
    class Ui
    {

      public:
        Ui() = delete;
        Ui(SystemInfoStaticData system_static_data, SystemInfoDataReceiver system_data_receiver,
           ProcessReceiver process_receiver);
        void Run();
        void PostEvent();

      private:
        std::shared_ptr<MainComponent> main_component_;
        ftxui::ScreenInteractive*      screen_;
    };

    struct ProcessesChannel
    {
        ProcessReceiver rx;
        ProcessSender   tx;

        ProcessesChannel();
    };

    struct SystemInfoDynChannel
    {
        SystemInfoDataReceiver rx;
        SystemInfoDataSender   tx;

        SystemInfoDynChannel();
    };

} // namespace riptop
