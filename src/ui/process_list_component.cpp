#include "ui/process_list_component.h"
#include "utils/utils.h"

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/string.hpp>

#include <algorithm>
#include <map>

using namespace riptop;

riptop::ColumnHeader::ColumnHeader(std::string name, ProcessSorting sorting, ProcessSorting selected_sorting,
                                   bool sort_order_ascending, std::string key, int16_t width)
    : sorting_(sorting), key_(key), width_(width)
{
    auto up    = "▲";
    auto down  = "▼";
    auto order = " ";
    if (sorting == selected_sorting)
    {
        width_--;
        order = sort_order_ascending ? up : down;
    }

    name_ = std::format("{}({}){}", name, key, order);
    if (width_ > 0)
    {
        name_ = std::format("{:>{}}({}){}", name, width, key, order);
    }
};



ProcessListComponent::ProcessListComponent(ProcessReceiver receiver) : receiver_(std::move(receiver)) {}


ftxui::Element ProcessListComponent::RenderProcesses()
{
    using namespace ftxui;

    std::vector<std::wstring> processes_to_display;

    while (receiver_->HasPending())
    {
        std::vector<ProcessInfo> incoming_infos;
        receiver_->Receive(&incoming_infos);
        process_count_ = incoming_infos.size();
        SortProcessList(&incoming_infos);
        processes_to_display = Format(incoming_infos);
    }
    if (!processes_to_display.empty())
        current_processes_ = processes_to_display;

    // prepare columns headers
    ColumnHeader pid("PID", ProcessSorting::Pid, sorting_, sort_order_ascending_, "i", 5);
    ColumnHeader user("USER", ProcessSorting::User, sorting_, sort_order_ascending_, "u", 7);
    ColumnHeader pri("PRI", ProcessSorting::Priority, sorting_, sort_order_ascending_, "r", 4);
    ColumnHeader cpu("CPU%", ProcessSorting::CpuPercent, sorting_, sort_order_ascending_, "c", 7);
    ColumnHeader mem("MEM", ProcessSorting::Memory, sorting_, sort_order_ascending_, "m", 5);
    ColumnHeader thrd("THRD", ProcessSorting::Thread, sorting_, sort_order_ascending_, "h", 0);
    ColumnHeader disk("DISK", ProcessSorting::DiskUsage, sorting_, sort_order_ascending_, "d", 5);
    ColumnHeader uptime("TIME", ProcessSorting::UpTime, sorting_, sort_order_ascending_, "t", 4);
    ColumnHeader proc("PROCESS", ProcessSorting::ProcessName, sorting_, sort_order_ascending_, "p", -1);

    auto header_text = std::format("{} {} {} {} {} {}       {} {}  {}", pid.name(), user.name(), pri.name(), cpu.name(),
                                   mem.name(), thrd.name(), disk.name(), uptime.name(), proc.name());
    auto header      = text(header_text) | bgcolor(Color::Green);

    // prepare list of processes
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

    return vbox(header, vbox(list) | vscroll_indicator | yframe);
}

bool ProcessListComponent::OnEvent(ftxui::Event event)
{
    // process list navigation
    auto old_selected = selected_;
    if (event == ftxui::Event::ArrowUp || event == ftxui::Event::Character("k"))
    {
        selected_--;
    }
    if (event == ftxui::Event::ArrowDown || event == ftxui::Event::Character("j"))
    {
        selected_++;
    }
    if (event == ftxui::Event::Home)
    {
        selected_ = 0;
    }
    if (event == ftxui::Event::End)
    {
        selected_ = process_count_ - 1;
    }
    if (selected_ != old_selected)
    {
        return true;
    }

    // column sorting
    if (event == ftxui::Event::Character("i"))
    {
        sorting_              = ProcessSorting::Pid;
        sort_order_ascending_ = !sort_order_ascending_;
    }
    if (event == ftxui::Event::Character("u"))
    {
        sorting_              = ProcessSorting::User;
        sort_order_ascending_ = !sort_order_ascending_;
    }
    if (event == ftxui::Event::Character("r"))
    {
        sorting_              = ProcessSorting::Priority;
        sort_order_ascending_ = !sort_order_ascending_;
    }
    if (event == ftxui::Event::Character("c"))
    {
        sorting_              = ProcessSorting::CpuPercent;
        sort_order_ascending_ = !sort_order_ascending_;
    }
    if (event == ftxui::Event::Character("m"))
    {
        sorting_              = ProcessSorting::Memory;
        sort_order_ascending_ = !sort_order_ascending_;
    }
    if (event == ftxui::Event::Character("h"))
    {
        sorting_              = ProcessSorting::Thread;
        sort_order_ascending_ = !sort_order_ascending_;
    }
    if (event == ftxui::Event::Character("d"))
    {
        sorting_              = ProcessSorting::DiskUsage;
        sort_order_ascending_ = !sort_order_ascending_;
    }
    if (event == ftxui::Event::Character("t"))
    {
        sorting_              = ProcessSorting::UpTime;
        sort_order_ascending_ = !sort_order_ascending_;
    }
    if (event == ftxui::Event::Character("p"))
    {
        sorting_              = ProcessSorting::ProcessName;
        sort_order_ascending_ = !sort_order_ascending_;
    }

    int size  = static_cast<int>(process_count_ <= 0 ? 0 : process_count_ - 1);
    selected_ = std::ranges::clamp(selected_, 0, size);

    return ftxui::ComponentBase::OnEvent(event);
}

void riptop::ProcessListComponent::SortProcessList(std::vector<ProcessInfo>* processes_to_sort)
{
    switch (sorting_)
    {
        case ProcessSorting::Pid:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (!sort_order_ascending_)
                {
                    return lhs.id > rhs.id;
                }
                else
                {
                    return lhs.id < rhs.id;
                }
            });
            break;
        case ProcessSorting::User:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (!sort_order_ascending_)
                {
                    return lhs.user_name > rhs.user_name;
                }
                else
                {
                    return lhs.user_name < rhs.user_name;
                }
            });
            break;
        case ProcessSorting::Priority:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (!sort_order_ascending_)
                {
                    return lhs.base_priority > rhs.base_priority;
                }
                else
                {
                    return lhs.base_priority < rhs.base_priority;
                }
            });
            break;
        case ProcessSorting::CpuPercent:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (!sort_order_ascending_)
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
                if (!sort_order_ascending_)
                {
                    return lhs.used_memory > rhs.used_memory;
                }
                else
                {
                    return lhs.used_memory < rhs.used_memory;
                }
            });
            break;
        case ProcessSorting::Thread:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (!sort_order_ascending_)
                {
                    return lhs.thread_count > rhs.thread_count;
                }
                else
                {
                    return lhs.thread_count < rhs.thread_count;
                }
            });
            break;
        case ProcessSorting::DiskUsage:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (!sort_order_ascending_)
                {
                    return lhs.disk_usage > rhs.disk_usage;
                }
                else
                {
                    return lhs.disk_usage < rhs.disk_usage;
                }
            });
            break;
        case ProcessSorting::UpTime:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (!sort_order_ascending_)
                {
                    return lhs.up_time > rhs.up_time;
                }
                else
                {
                    return lhs.up_time < rhs.up_time;
                }
            });
            break;
        case ProcessSorting::ProcessName:
            std::sort(processes_to_sort->begin(), processes_to_sort->end(), [=](const auto& lhs, const auto& rhs) {
                if (!sort_order_ascending_)
                {
                    return lhs.exe_name > rhs.exe_name;
                }
                else
                {
                    return lhs.exe_name < rhs.exe_name;
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
        // "{:>7d} {:>9} {:>3d} {:>3.1f}% {:>10} {:>6d} {:>6d} MB/s {:>%H:%M:%S} {}"
        // "{:>7} {:>11} {:>8} {:>10}% {:>10} {:>6} {:>11} {:>8} {}", "PID(i)", "USER(u)", "PRI",
        //                 "CPU(c)", "MEM(m)", "THREAD", "DISK", "TIME", "PROCESS(p)"
        auto row = ftxui::to_wstring(
            //"PID(i)", "USER(u)", "PRI", "CPU(c)", "MEM(m)", "THREAD", "DISK", "TIME", "PROCESS(p)"
            std::format("{:>7d} {:>11} {:>8d} {:>10.1f}% {:>10}   {:>6d}     {:>6d} MB/s {:>8%H:%M:%S}   {}",
                        process.id, process.user_name, process.base_priority, process.percent_processor_time,
                        riptop::FormatMemory(process.used_memory), process.thread_count, process.disk_usage,
                        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(process.up_time)),
                        process.exe_name));
        lines.push_back(row);
    }
    return lines;
}
