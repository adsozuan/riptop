#include "../../include/ui/main_ui.h"
#include "../../include/probes/memory_usage_info.h"
#include "../../include/probes/system_info.h"
#include "../../include/probes/system_times.h"
#include "../../include/utils/formatter.h"

#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/table.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"
#include <ftxui/component/component.hpp>

void RenderMainUi()
{
    using namespace ftxui;
    using namespace riptop;

    auto title = Renderer([&] {
        auto content = vbox(text("riptop") | hcenter) | bgcolor(Color::Blue);
        return (content);
    });

    auto usage_gauge = [&](std::string name, float value) {
        std::ostringstream value_stream;
        value_stream << std::setprecision(3) << value * 100;
        auto content = hbox(text(name) | color(Color::Cyan3), text("["), gauge(value) | color(Color::Green) | flex,
                            text(value_stream.str()) | color(Color::GrayDark), text("%") | color(Color::GrayDark),
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
               }) |
               notflex;
    };

    auto system_info_area = [&](const MemoryUsageInfo& mem_info) {
        SystemInfo system_info;

        return vbox({
            hbox(text("Tasks: ") | color(Color::Cyan3), text("290 total, 5 running")),
            hbox(text("Size: ") | color(Color::Cyan3), text(format_memory(mem_info.total_memory()))),
            hbox(text("Uptime: ") | color(Color::Cyan3), text(system_info.GetUptime())),
        });
    };

    auto global_usage = Renderer([&] {
        MemoryUsageInfo mem_info;

        auto content = hbox(
            {usage_gauges_area(mem_info), vbox({separator(), separator(), separator()}), system_info_area(mem_info)});
        return content;
    });

    auto process_table_renderer = Renderer([&] {
        auto process_table = Table({
            {"ID", "USER", "PRI", "CPU%", "MEM", "THREAD", "DISK", "TIME", "PROCESS"},
            {"25460", "ad", "4", "03.1%", "120.3 MB", "29", "0.0 MB/s", "00:01:30:39", "firefox.exe"},
            {"25348", "ad", "8", "13.1%", "320.4 MB", "19", "0.0 MB/s", "00:01:40:11", "chrome.exe"},
            {"25148", "ad", "4", "01.1%", "20.3 MB", "9", "0.0 MB/s", "00:01:45:11", "jezibella.exe"},
            {"25111", "adnotnumber", "4", "11.1%", "20.3 MB", "4", "11.0 MB/s", "00:01:58:57",
             "wonderful_long_application.exe"},
        });

        process_table.SelectColumns(0, 7).DecorateCells(align_right);
        process_table.SelectCell(0, 0).DecorateCells(bgcolor(Color::CyanLight));
        process_table.SelectRow(0).SeparatorVertical(EMPTY);
        process_table.SelectRow(0).Decorate(bgcolor(Color::Green));
        process_table.SelectRow(0).Decorate(color(Color::Black));

        auto content = process_table.SelectRows(1, -1);
        content.DecorateCellsAlternateRow(color(Color::Cyan), 2, 0);
        content.DecorateCellsAlternateRow(color(Color::White), 2, 1);
        return vbox({process_table.Render() | vscroll_indicator | frame});
    });

    auto screen = ScreenInteractive::FitComponent();

    auto renderer = Renderer([&] {
        return vbox({title->Render(), separator(), global_usage->Render(), separator(),
                     process_table_renderer->Render()}) |
               size(WIDTH, GREATER_THAN, 80) | border;
    });

    int shift {0};

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
