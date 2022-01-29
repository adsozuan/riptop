#include "ui/process_list_component.h"
#include "utils/utils.h"

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/string.hpp>

#include <algorithm>

using namespace riptop;

ProcessListComponent::ProcessListComponent(ProcessReceiver receiver) : receiver_(std::move(receiver)) {}

ftxui::Element ProcessListComponent::RenderProcesses()
{
    using namespace ftxui;

    std::vector<std::wstring> processes_to_display;

    while (receiver_->HasPending())
    {
        std::vector<ProcessInfo> incoming_infos;
        receiver_->Receive(&incoming_infos);
        process_count_       = incoming_infos.size();
        processes_to_display = Format(incoming_infos);
    }
    if (!processes_to_display.empty())
        current_processes_ = processes_to_display;

    auto r = std::format("{:>7} {:>9} {:>3} {:>4}% {:>10} {:>6} {:>11} {:>8} {}", "PID", "USER", "PRI", "CPU", "MEM",
                         "THREAD", "DISK", "TIME", "PROCESS");
    auto header = text(r) | bgcolor(Color::Green);

    Elements list;
    int      index {0};

    for (const auto& process : current_processes_)
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
    process_count_ = current_processes_.size();
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
    int size  = process_count_ <= 0 ? 0 : process_count_ - 1;
    selected_ = std::ranges::clamp(selected_, 0, size);

    return ftxui::ComponentBase::OnEvent(event);
}

std::vector<std::wstring> ProcessListComponent::Format(const std::vector<ProcessInfo>& processes)
{
    std::vector<std::wstring> lines;

    int index {0};
    for (auto& process : processes)
    {
        auto row = ftxui::to_wstring(
            std::format("{:>7d} {:>9} {:>3d} {:>3.1f}% {:>10} {:>6d} {:>6d} MB/s {:>%H:%M:%S} {}", process.id,
                        process.user_name, process.base_priority, process.percent_processor_time,
                        riptop::FormatMemory(process.used_memory), process.thread_count, process.disk_usage,
                        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(process.up_time)),
                        process.exe_name));
        lines.push_back(row);
    }
    return lines;
}
