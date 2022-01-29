#include "ui/main_component.h"

#include <ftxui/component/event.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/string.hpp>

using namespace riptop;

MainComponent::MainComponent(ProcessReceiver process_receiver, SystemInfoStaticData system_info_static_data,
                             SystemInfoDataReceiver system_data_receiver)
    : computer_name_(system_info_static_data.computer_name)
{
    process_table_ = std::make_shared<ProcessListComponent>(std::move(process_receiver));
    system_info_   = std::make_shared<SystemInfoComponent>(system_info_static_data, std::move(system_data_receiver));
    Add(ftxui::Container::Vertical({system_info_, process_table_}));
}

ftxui::Element MainComponent::Render()
{
    using namespace ftxui;

    auto title_value = std::format("riptop on {}", computer_name_);
    auto title       = hbox(text(title_value)) | hcenter | bgcolor(Color::Blue);

    process_table_->TakeFocus();

    return vbox(
        {
            title , 
			separatorEmpty(), 
			system_info_->Render() | notflex, 
			separatorEmpty(), 
			process_table_->RenderProcesses() | yflex
        });
}

bool MainComponent::OnEvent(ftxui::Event event) { return ComponentBase::OnEvent(event); }