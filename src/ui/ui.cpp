#include "ui/ui.h"


#include "ui/system_info_component.h"
#include "ui/process_list_component.h"

riptop::Ui::Ui(SystemInfoStaticData system_static_data, SystemInfoDataReceiver system_data_receiver, ProcessReceiver process_receiver) {

    main_component_ = std::make_shared<MainComponent>(std::move(process_receiver), system_static_data,
                                                     std::move(system_data_receiver));

}

void riptop::Ui::Run() {

    auto screen = ftxui::ScreenInteractive::FitComponent();
    screen_     = &screen;
    screen.Loop(main_component_);
}

void riptop::Ui::PostEvent() { screen_->PostEvent(ftxui::Event::Custom); }
