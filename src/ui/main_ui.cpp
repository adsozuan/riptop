#include "../../include/ui/main_ui.h"
#include "../../include/probes/memory_usage_info.h"
#include "../../include/probes/process_list.h"
#include "../../include/probes/system_info.h"
#include "../../include/probes/system_times.h"
#include "../../include/utils/formatter.h"

#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/table.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"
#include "ftxui/component/component.hpp"

#include <format>

// clang-format off

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

void RenderMainUi()
{
    using namespace ftxui;
    using namespace riptop;


    auto title = Renderer([&] {
        auto content = vbox(text("riptop") | hcenter) | bgcolor(Color::Blue);
        return (content);
    });

    auto usage_gauge = [&](std::string name, float value) {
        
        ftxui::Color::Palette256 gauge_color = Color::Green1;

        if (value > 0.5 ) {
            gauge_color = Color::Orange1;
        }
        else if (value > 0.75)
        {
             gauge_color = Color::Red1;
        }
        auto content = hbox(text(name) | color(Color::Cyan3), text("["), gauge(value) | color(gauge_color) | flex,
                            text(std::format("{:2.1f}", value * 100)) | color(Color::GrayDark), text("%") | color(Color::GrayDark),
                            text("]") | size(WIDTH, EQUAL, 5));
        return content;
    };

    auto usage_gauges_area = [&](const MemoryUsageInfo& mem_info) {
        SystemTimes system_times;
        system_times.UpdateCpuUsage();

        return vbox({
                   usage_gauge("CPU", static_cast<float>(system_times.cpu_usage())),
                   usage_gauge("MEM", static_cast<float>(mem_info.used_memory_percentage())),
                   usage_gauge("PGE", static_cast<float>(mem_info.used_page_memory_percentage())),
               }) | flex;
    };
    

    auto system_info_area = [&](const MemoryUsageInfo& mem_info) {
        SystemInfo system_info;

        return vbox({
            hbox(text("Tasks: ")  | color(Color::Cyan3), text("290 total, 5 running")),
            hbox(text("Size: ")   | color(Color::Cyan3), text(format_memory(mem_info.total_memory()))),
            hbox(text("Uptime: ") | color(Color::Cyan3), text(system_info.GetUptime())),
            hbox(text("Proc: ")   | color(Color::Cyan3), text(system_info.processor_name())),
        }) | flex;
    };
    


    auto global_usage = Renderer([&] {
        MemoryUsageInfo mem_info;

        auto content = hbox({
            usage_gauges_area(mem_info), 
            filler(),
            system_info_area(mem_info)
            }) ;
        return content;
    });

    std::vector<std::vector<std::string>> outputs(65, std::vector<std::string>(9, ""));
    outputs[0] = {"PID", "USER", "PRI", "CPU%", "MEM", "THREAD", "DISK", "TIME", "PROCESS"};
    auto process_table_renderer = Renderer([&] {
        ProcessList process_list;
        process_list.UpdateProcessList(100);
        ProcessListToTable(outputs, process_list.processes());
        auto process_table = Table(outputs);

        process_table.SelectColumns(0, 7).DecorateCells(align_right);
        process_table.SelectCell(0, 0).DecorateCells(bgcolor(Color::CyanLight));
        process_table.SelectRow(0).SeparatorVertical(EMPTY);
        process_table.SelectRow(0).Decorate(bgcolor(Color::Green));
        process_table.SelectRow(0).Decorate(color(Color::Black));

        auto content = process_table.SelectRows(1, -1);
        content.DecorateCellsAlternateRow(color(Color::Cyan), 2, 0);
        content.DecorateCellsAlternateRow(color(Color::White), 2, 1);
        return vbox({process_table.Render() | vscroll_indicator | frame | border });
    });

    auto screen = ScreenInteractive::FitComponent();

    auto renderer = Renderer([&] {
        return vbox({
                     title->Render(), 
                     separator(), 
                     global_usage->Render(), 
                     separator(),
                     process_table_renderer->Render()
                   }); 
    });

    int         shift {0};
    bool        refresh_ui_continue = true;
    std::thread refresh_ui([&] {
        while (refresh_ui_continue)
        {
            using namespace std::chrono_literals;
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
