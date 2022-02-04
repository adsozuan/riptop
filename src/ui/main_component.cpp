#include "ui/main_component.h"

#include <ftxui/component/event.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/string.hpp>

using namespace riptop;

MainComponent::MainComponent(std::atomic<bool>* quit, ProcessReceiver process_receiver,
                             SystemInfoStaticData system_info_static_data, SystemInfoDataReceiver system_data_receiver)
    : computer_name_(system_info_static_data.computer_name), should_quit_(quit)
{
    process_table_ = std::make_shared<ProcessListComponent>(std::move(process_receiver));
    system_info_   = std::make_shared<SystemInfoComponent>(system_info_static_data, std::move(system_data_receiver));
    Add(ftxui::Container::Vertical({system_info_, process_table_}));
}

ftxui::Element MainComponent::Render()
{
    using namespace ftxui;

    auto help_prompt = show_help_ ? "Press 'x' to close help" : "Press 'x' to show help";

    auto title_value = std::format("riptop on {} - {}", computer_name_, help_prompt);
    auto title       = hbox(text(title_value)) | hcenter | bgcolor(Color::Blue);
    if (show_help_)
    {
        auto help_table  = Table({
            {"FUNCTIONS", "KEYS", ""},
            {"Exit ", " Press 'q'"},
            {"Movement Up ", " Press UP or 'k'"},
            {"Movement Down ", " Press DOWN or 'j'"},
            {"Movement Top ", " Press HOME"},
            {"Movement Bottom ", " Press END"},
            {"Column sorting ", " Press key in parenthesis in column name."},
            {"Column sorting ", " Press again to change sorting order."},
            {"Process search ", " Press /, enter process name and press RETURN. ESCAPE to leave Search."},
        });
        help_table.SelectRow(0).Decorate(bold);
        help_table.SelectRow(0).SeparatorVertical(LIGHT);
        help_table.SelectRow(0).Border(DOUBLE);
        help_table.SelectColumn(0).Border(LIGHT);
        help_table.SelectAll().Border(LIGHT);
        auto help_window = window(text("HELP (x to close)"), help_table.Render());

        auto help = hflow(help_window | hcenter | bgcolor(Color::Blue)) | yflex | border;
        return vbox(
            {title, separatorEmpty(), help | notflex, separatorEmpty(), process_table_->RenderProcesses() | yflex});
    }
    else
    {
        process_table_->TakeFocus();
        return vbox({title, separatorEmpty(), system_info_->Render() | notflex, separatorEmpty(),
                     process_table_->RenderProcesses() | yflex});
    }
}

bool MainComponent::OnEvent(ftxui::Event event)
{
    if (event == ftxui::Event::Character("q"))
    {
        should_quit_->store(true); // for stoping Acquisition thread
        OnQuit();                  // for stop Screen Loop
    }
    if (event == ftxui::Event::Character("x"))
    {
        show_help_ = !show_help_;
    }

    return ComponentBase::OnEvent(event);
}
