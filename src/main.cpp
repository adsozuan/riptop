
#include <iomanip>
#include <iostream>
#include <sstream>

#include "../include/probes/memory_usage_info.h"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/table.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"

int main(void) {
  using namespace ftxui;

  auto summary = [&] {
    auto content = vbox({
        hbox({text(L"- done:   "), text(L"3") | bold}) | color(Color::Green),
        hbox({text(L"- active: "), text(L"2") | bold}) | color(Color::RedLight),
        hbox({text(L"- queue:  "), text(L"9") | bold}) | color(Color::Red),
    });
    return window(text(L" Summary "), content);
  };

  auto title = [&] {
    auto content = vbox(text("riptop") | hcenter) | bgcolor(Color::Blue);
    return (content);
  };

  auto usage_gauge = [&](std::string name, float value) {
    std::ostringstream value_stream;
    value_stream << std::setprecision(3) << value * 100;
    auto content = hbox(text(name) | color(Color::Cyan3), text("["),
                        gauge(value) | color(Color::Green) | flex,
                        text(value_stream.str()) | color(Color::GrayDark),
                        text("%") | color(Color::GrayDark), text("]"));
    return content;
  };

  auto global_usage = [&] {
    MemoryUsageInfo mem_info;
    auto total_mem = mem_info.total_memory_GB();
    std::ostringstream total_mem_stream;
    total_mem_stream << std::setprecision(3) << total_mem;
    auto content =
        hbox({vbox({
                  usage_gauge("CPU", 0.12),
                  usage_gauge("MEM", mem_info.used_memory_percentage()),
                  usage_gauge("PGE", mem_info.used_page_memory_percentage()),
              }) | flex,
              vbox({separator(), separator(), separator()}),
              vbox({
                  hbox(text("Tasks: ") | color(Color::Cyan3),
                       text("290 total, 5 running")),
                  hbox(text("Size: ") | color(Color::Cyan3),
                       text(total_mem_stream.str()), text("GB")),
                  hbox(text("Updatime: ") | color(Color::Cyan3),
                       text("18:02:23:15")),
              }) | flex});
    return content;
  };

  auto process_table = Table({
      {"ID", "USER", "PRI", "CPU%", "MEM", "THREAD", "DISK", "TIME", "PROCESS"},
      {"25460", "ad", "4", "03.1%", "120.3 MB", "29", "0.0 MB/s", "00:01:30:39",
       "firefox.exe"},
      {"25348", "ad", "8", "13.1%", "320.4 MB", "19", "0.0 MB/s", "00:01:40:11",
       "chrome.exe"},
      {"25148", "ad", "4", "01.1%", "20.3 MB", "9", "0.0 MB/s", "00:01:45:11",
       "jezibella.exe"},
      {"25111", "adnotnumber", "4", "11.1%", "20.3 MB", "4", "11.0 MB/s",
       "00:01:58:57", "wonderful_long_application.exe"},
  });

  process_table.SelectColumns(0, 7).DecorateCells(align_right);
  process_table.SelectCell(0, 0).DecorateCells(bgcolor(Color::CyanLight));
  process_table.SelectRow(0).SeparatorVertical(EMPTY);
  process_table.SelectRow(0).Decorate(bgcolor(Color::Green));
  process_table.SelectRow(0).Decorate(color(Color::Black));

  auto content = process_table.SelectRows(1, -1);
  content.DecorateCellsAlternateRow(color(Color::Cyan), 2, 0);
  content.DecorateCellsAlternateRow(color(Color::White), 2, 1);

  auto document =  //
      vbox({title(), separator(), global_usage(), separator(),
            vbox({center(process_table.Render()), separator()})});

  // Limit the size of the document to 80 char.
  document = document | size(WIDTH, LESS_THAN, 80);

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);

  std::cout << screen.ToString() << std::endl;

  return EXIT_SUCCESS;
}
