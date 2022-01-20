#include "..\..\include\ui\process_list_component.h"

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/string.hpp>

ProcessListComponent::ProcessListComponent(ProcessReceiver receiver) : receiver_(std::move(receiver))
{
    processes_.reserve(300);
}

ftxui::Element ProcessListComponent::RenderProcesses()
{

    using namespace ftxui;

    auto r = std::format("{:>7} {:>9} {:>3} {:>4}% {:>10} {:>6} {:>11} {:>8} {}", "PID", "USER", "PRI", "CPU", "MEM",
                         "THREAD", "DISK", "TIME", "PROCESS");
    auto header = text(r) | bgcolor(Color::Green);

    std::vector<std::string> outputs(processes_);
    auto lines = Menu(&outputs, &selected_);

    return vbox({header, separatorEmpty(), lines->Render()| vscroll_indicator | frame | flex});
}

bool ProcessListComponent::OnEvent(ftxui::Event event)
{
    while (receiver_->HasPending())
    {
        std::vector<ProcessInfo> incoming_infos;
        receiver_->Receive(&incoming_infos);
        Format(incoming_infos);
    }
    auto old_selected = selected_;
    if (event == ftxui::Event::ArrowUp || event == ftxui::Event::Character("k"))
    {
        selected_--;
    }
    if (event == ftxui::Event::ArrowDown || event == ftxui::Event::Character("j"))
    {
        selected_++;
    }
    if (selected_ != old_selected)
    {
        return true;
    }

    return ftxui::ComponentBase::OnEvent(event);
}

void ProcessListComponent::Format(const std::vector<ProcessInfo>& processes)
{


    auto process_count = processes.size();
    if (process_count > processes_.size())
    {
        processes_.resize(process_count);
    }

    int index {0};
    for (auto& process : processes)
    {
        auto row =
            std::format("{:>7d} {:>9} {:>3d} {:>4.1g}% {:>7d} MB {:>6d} {:>6d} MB/s {:>%H:%M:%S} {}", process.id,
                        process.user_name, process.base_priority, process.percent_processor_time, process.used_memory,
                        process.thread_count, process.disk_usage,
                        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(process.up_time)),
                        process.exe_name);
        processes_[index] = row;
        index++;
    }
}
