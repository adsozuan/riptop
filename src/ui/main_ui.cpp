#include "../../include/ui/main_ui.h"
#include "../../include/probes/memory_usage_info.h"
#include "../../include/probes/process_list.h"
#include "../../include/probes/system_info.h"
#include "../../include/probes/system_times.h"
#include "../../include/utils/formatter.h"


#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/table.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"

#include <format>

// clang-format off

using namespace ftxui;
using namespace riptop;

void ProcessListToTable(std::vector<std::vector<std::string>>& outputs, const std::vector<Process>& processes)
{
    int line_number = 1; // 0 is allocated to column header
    for (auto& process : processes)
    {
        auto& line = outputs[line_number];
        line[0]    = std::format("{:d}", process.id);
        line[1]    = std::format("{}", process.user_name);
        line[2]    = std::format("{:d}", process.base_priority);
        line[3]    = std::format("{:g}%", process.percent_processor_time);
        line[4]    = std::format("{:d} MB", process.used_memory);
        line[5]    = std::format("{:d}", process.thread_count);
        line[6]    = std::format("{:d} MB/s", process.disk_usage);
        line[7] =
            std::format("{:%H:%M:%S}",
                        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(process.up_time)));
        line[8] = std::format("{}", process.exe_name);
        line_number++;
    }
}

Component MakeProcessTable(std::vector<std::string>& processes, Component component) 
{
  return Renderer([processes, component] {
    return hbox({
               component->Render() |  vscroll_indicator | frame | yflex,
           }) |
           yflex;
  });
}


void riptop::RenderMainUi()
{

    SystemInfo system_info;
    SystemTimes system_times;
    MemoryUsageInfo mem_info;
    ProcessList process_list;

    auto title = Renderer([&] {
        auto title_value = std::format("riptop on {}", system_info.computer_name());
        auto content = hbox(text(title_value)) | hcenter | bgcolor(Color::Blue);
        return content;
    });

    // system usageg & informations
    auto usage_gauge = [&](std::string name, float value) {
        
        ftxui::Color::Palette256 gauge_color = Color::Green1;

        if (value > 0.5 ) {
            gauge_color = Color::Orange1;
        }
        if (value > 0.75)
        {
             gauge_color = Color::Red1;
        }
        auto content = hbox(separatorEmpty(), text(name) | color(Color::Cyan3), text("["), gauge(value) | color(gauge_color) | flex,
                            text(std::format("{:2.1f}", value * 100)) | color(Color::GrayDark), text("%") | color(Color::GrayDark),
                            text("]") ) | size(WIDTH, EQUAL, 35);
        return content;
    };

    auto usage_gauges_area = [&](const MemoryUsageInfo& mem_info) {
        return vbox({
                   usage_gauge("CPU", static_cast<float>(system_times.cpu_usage())),
                   usage_gauge("MEM", static_cast<float>(mem_info.used_memory_percentage())),
                   usage_gauge("PGE", static_cast<float>(mem_info.used_page_memory_percentage())),
               }) ;
    };
    
    auto system_info_area = [&](const MemoryUsageInfo& mem_info) {

        return vbox({
             hbox(separatorEmpty(), text("Tasks: ")  | color(Color::Cyan3), text("290 total, 5 running")),
             hbox(separatorEmpty(), text("Size: ")   | color(Color::Cyan3), text(format_memory(mem_info.total_memory()))),
             hbox(separatorEmpty(), text("Uptime: ") | color(Color::Cyan3), text(system_info.GetUptime())),
             hbox(separatorEmpty(), text("Proc: ")   | color(Color::Cyan3), text(system_info.processor_name())),
        }) ;
    };
    
    auto global_usage = Renderer([&] {

        auto content = hbox({
            usage_gauges_area(mem_info), 
            filler(),
            system_info_area(mem_info)
            }) | xflex_grow;
        return content;
    });

    // process table
    std::vector<std::vector<std::string>> outputs(ProcessList::PROCESS_MAX_NUMBER + 1, std::vector<std::string>(9, ""));
    outputs[0] = {"PID", "USER", "PRI", "CPU%", "MEM", "THREAD", "DISK", "TIME", "PROCESS"};

    auto process_table_renderer = Renderer([&] {
        auto process_table = Table(outputs);
        ProcessListToTable(outputs, process_list.processes());

        // set columns width
        std::vector<int> columns_width = {7, 9, 3, 5, 11, 4, 9, 8, 30};
        int col {0};
        for (auto column_width : columns_width)
		{
            process_table.SelectColumn(col).DecorateCells(size(WIDTH, Constraint::EQUAL, column_width));
            col++;
		}

        process_table.SelectColumns(0, 7).DecorateCells(align_right);
        process_table.SelectCell(0, 0).DecorateCells(bgcolor(Color::CyanLight));
        process_table.SelectRow(0).SeparatorVertical(EMPTY);
        process_table.SelectRow(0).Decorate(bgcolor(Color::Green));
        process_table.SelectRow(0).Decorate(color(Color::Black));

        auto content = process_table.SelectRows(1, -1);
        content.DecorateCellsAlternateRow(color(Color::Cyan), 2, 0);
        content.DecorateCellsAlternateRow(color(Color::White), 2, 1);
        return vbox({process_table.Render() | vscroll_indicator | frame | flex_grow });
    });

    int line_selected = 3;
    process_list.UpdateProcessList(100);

    std::vector<std::string> processes; 
    processes.resize(ProcessList::PROCESS_MAX_NUMBER+1);

	auto r = std::format("{:>7} {:>9} {:>3} {:>4}% {:>10} {:>6} {:>11} {:>8} {}", 
        "PID", "USER", "PRI", "CPU", "MEM", "THREAD", "DISK", "TIME", "PROCESS"
        );

    processes[0] = r;

    process_list.FormatToProcessesRows(processes);

    auto lines = Menu(&processes, &line_selected);
    lines->TakeFocus();

    auto lines_component = MakeProcessTable(processes, lines);

    auto container = Container::Vertical({
        lines_component
        });

    // Main screen, renderer and ui refresh thread
    auto screen = ScreenInteractive::FitComponent();

    auto renderer = Renderer(container, [&] {
        return vbox({
                     title->Render(), 
                     separatorEmpty(), 
                     global_usage->Render() , 
                     separatorEmpty(), 
                     lines_component->Render() | flex
                   }) | flex; 
    });

    int         shift {0};
    bool        refresh_ui_continue = true;
    std::thread refresh_ui([&] {
        while (refresh_ui_continue)
        {
            using namespace std::chrono_literals;

            system_info.Update();
            system_times.UpdateCpuUsage();
            mem_info.Update();
            process_list.UpdateProcessList(100);
			process_list.FormatToProcessesRows(processes);

            std::this_thread::sleep_for(500ms);
            shift++;
            screen.PostEvent(Event::Custom);
        }
    });

    screen.Loop(renderer);
    refresh_ui_continue = false;
    refresh_ui.join();
}

// clang-format on 
