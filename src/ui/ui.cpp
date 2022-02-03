#include "ui/ui.h"
#include "ui/system_info_component.h"
#include "ui/process_list_component.h"

riptop::Ui::Ui(std::atomic<bool>* quit, SystemInfoStaticData system_static_data, SystemInfoDataReceiver system_data_receiver,
               ProcessReceiver process_receiver)
{

    main_component_ = std::make_shared<MainComponent>(quit, std::move(process_receiver), system_static_data,
                                                      std::move(system_data_receiver));
}

void riptop::Ui::Run()
{

    auto screen = ftxui::ScreenInteractive::FitComponent();
    screen_     = &screen;
    main_component_->OnQuit = screen_->ExitLoopClosure();

    screen.Loop(main_component_);
}

void riptop::Ui::PostEvent() { screen_->PostEvent(ftxui::Event::Custom); }

riptop::ProcessesChannel::ProcessesChannel()
{

    rx = ftxui::MakeReceiver<std::vector<riptop::ProcessInfo>>();
    tx = rx->MakeSender();
}

riptop::SystemInfoDynChannel::SystemInfoDynChannel()
{

    rx = ftxui::MakeReceiver<riptop::SystemInfoDynamicData>();
    tx = rx->MakeSender();
}
