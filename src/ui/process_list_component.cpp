#include "..\..\include\ui\process_list_component.h"
#include "../../include/utils/formatter.h"

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/string.hpp>

#include <algorithm>

ProcessListComponent::ProcessListComponent(ProcessReceiver receiver) : receiver_(std::move(receiver))
{
    processes_.resize(32);
}

ftxui::Element ProcessListComponent::RenderProcesses()
{
    using namespace ftxui;

    while (receiver_->HasPending())
    {
        std::vector<ProcessInfo> incoming_infos;
        receiver_->Receive(&incoming_infos);
        Format(std::move(incoming_infos));
    }

    auto r = std::format("{:>7} {:>9} {:>3} {:>4}% {:>10} {:>6} {:>11} {:>8} {}", "PID", "USER", "PRI", "CPU", "MEM",
                         "THREAD", "DISK", "TIME", "PROCESS");
    auto header = text(r) | bgcolor(Color::Green);

    Elements list;
    int      index {0};

    for (auto& process : processes_)
    {

        bool has_focus = (index++ == selected_);

        Decorator line_selector = nothing;
        if (has_focus)
        {
            line_selector = line_selector | focus | inverted;
        }
        auto line = hbox(text(process)) | line_selector;

        list.push_back(line);
    }

    return vbox({header, separatorEmpty(), vbox(list) | vscroll_indicator | yframe});
}

bool ProcessListComponent::OnEvent(ftxui::Event event)
{
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
    int size  = processes_.size() - 1;
    selected_ = std::ranges::clamp(selected_, 0, size);

    return ftxui::ComponentBase::OnEvent(event);
}

void ProcessListComponent::Format(const std::vector<ProcessInfo>& processes)
{

    auto process_count = processes.size();
    int  index {0};

    for (auto& process : processes)
    {
        auto row = ftxui::to_wstring(
            std::format("{:>7d} {:>9} {:>3d} {:>4.1g}% {:>10} {:>6d} {:>6d} MB/s {:>%H:%M:%S} {}", process.id,
                        process.user_name, process.base_priority, process.percent_processor_time,
                        riptop::format_memory(process.used_memory), process.thread_count, process.disk_usage,
                        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(process.up_time)),
                        process.exe_name));

        if (process_count >= processes_.size()) // more processes are incoming than available rooms
        {
            processes_.push_back(row);
        }
        else // enough rooms for incoming processes
        {
            processes_[index] = row;
        }
        index++;
    }
}
