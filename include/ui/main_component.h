#pragma once

#include "process_list_component.h"
#include "system_info_component.h"

#include <ftxui/component/component.hpp>
#include <ftxui/component/receiver.hpp>

namespace riptop
{
    class MainComponent : public ftxui::ComponentBase
    {
      public:
        MainComponent(std::atomic<bool>* quit, ProcessReceiver process_receiver,
                      SystemInfoStaticData system_info_static_data, SystemInfoDataReceiver system_data_receiver);
        ftxui::Element        Render() override;
        bool                  OnEvent(ftxui::Event event) override;
        std::function<void()> OnQuit = []() {};

      private:
        std::atomic<bool>*                    should_quit_ {};
        std::string                           computer_name_;
        std::shared_ptr<ProcessListComponent> process_table_ = nullptr;
        std::shared_ptr<SystemInfoComponent>  system_info_   = nullptr;
    };
} // namespace riptop
