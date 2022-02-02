#include "ui/process_list_component.h"
#include "utils/utils.h"

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/string.hpp>

#include <algorithm>
#include <map>

using namespace riptop;

ProcessListComponent::ProcessListComponent(ProcessReceiver receiver) : receiver_(std::move(receiver)) {}

bool IsAscending() { return false;

}

ftxui::Element ProcessListComponent::RenderProcesses()
{
    using namespace ftxui;

    std::vector<std::wstring> processes_to_display;

    while (receiver_->HasPending())
    {
        std::vector<ProcessInfo> incoming_infos;
        receiver_->Receive(&incoming_infos);
        process_count_ = incoming_infos.size();
        SortProcessList(&incoming_infos, sorting_, sort_order_);
        processes_to_display = Format(incoming_infos);
    }
    if (!processes_to_display.empty())
        current_processes_ = processes_to_display;

    std::map<ProcessSorting, std::string> headers {
        {ProcessSorting::Pid, "PID(i)"},
        {ProcessSorting::User, "USER(u)"},
        {ProcessSorting::Priority, "PRI"},
        {ProcessSorting::CpuPercent, "CPU(c)"},
        {ProcessSorting::Memory, "MEM(m)"},
        {ProcessSorting::Thread, "THREAD"},
        {ProcessSorting::DiskUsage, "DISK"},
        {ProcessSorting::UpTime, "TIME"},
        {ProcessSorting::ProcessName, "PROCESS(p)"},
    };

    //for (auto& col : headers)
    //{
    //    if (col.first == sorting_)
    //    {
    //        col.second.append("/\\");
    //    }
    //    else
    //    {
    //        if (col.second.find("/\\") > 1)
    //            col.second.replace(col.second.find("/\\"), 2, "");
    //    }
    //}

    auto r      = std::format("{:>7} {:>11} {:>8} {:>10}% {:>10} {:>6} {:>11} {:>8} {}", "PID(i)", "USER(u)", "PRI",
                         "CPU(c)", "MEM(m)", "THREAD", "DISK", "TIME", "PROCESS(p)");
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
    // SELECTION
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

    // SORTING
    if (event == ftxui::Event::Character("c"))
    {
        sorting_ = ProcessSorting::CpuPercent;
    }
    if (event == ftxui::Event::Character("m"))
    {
        sorting_ = ProcessSorting::Memory;
    }
    if (event == ftxui::Event::Character("i"))
    {
        sorting_ = ProcessSorting::Pid;
    }
    if (event == ftxui::Event::Character("u"))
    {
        sorting_ = ProcessSorting::User;
    }
    if (event == ftxui::Event::Character("t"))
    {
        sorting_ = ProcessSorting::UpTime;
    }
    if (event == ftxui::Event::Character("p"))
    {
        sorting_ = ProcessSorting::ProcessName;
    }

    int size  = static_cast<int>(process_count_ <= 0 ? 0 : process_count_ - 1);
    selected_ = std::ranges::clamp(selected_, 0, size);

    return ftxui::ComponentBase::OnEvent(event);
}
void riptop::ProcessListComponent::SortProcessList(std::vector<ProcessInfo>* processes_to_sort,
                                                   ProcessSorting sort_type, SortOrder sort_order)
{
    switch (sort_type)
    {
        case ProcessSorting::CpuPercent:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (sort_order == SortOrder::Descending)
                {
                    return lhs.percent_processor_time > rhs.percent_processor_time;
                }
                else
                {
                    return lhs.percent_processor_time < rhs.percent_processor_time;
                }
            });
            break;
        case ProcessSorting::Memory:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (sort_order == SortOrder::Descending)
                {
                    return lhs.used_memory > rhs.used_memory;
                }
                else
                {
                    return lhs.used_memory < rhs.used_memory;
                }
            });
            break;
        case ProcessSorting::Pid:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (sort_order == SortOrder::Descending)
                {
                    return lhs.id > rhs.id;
                }
                else
                {
                    return lhs.used_memory < rhs.used_memory;
                }
            });
            break;
        case ProcessSorting::ProcessName:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (sort_order == SortOrder::Descending)
                {
                    return lhs.exe_name > rhs.exe_name;
                }
                else
                {
                    return lhs.exe_name < rhs.exe_name;
                }
            });
            break;
        case ProcessSorting::User:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (sort_order == SortOrder::Descending)
                {
                    return lhs.user_name > rhs.user_name;
                }
                else
                {
                    return lhs.user_name < rhs.user_name;
                }
            });
            break;
    }
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
