#include <windows.h>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/table.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"

constexpr auto to_mega_bytes(int64_t x) {
  return x / 1048576;
}
class MemoryUsageInfo {
 public:
  MemoryUsageInfo() {
    memory_info_.dwLength = sizeof(MEMORYSTATUSEX);
    Update();
  };

  int64_t total_memory() { return total_memory_; }
  int64_t total_memory_GB() { return total_memory_ / 1000; }
  int64_t used_memory() { return used_memory_; }
  double used_memory_percentage() { return used_memory_percentage_; }

  void Update() {
    GlobalMemoryStatusEx(&memory_info_);
    total_memory_ = to_mega_bytes(memory_info_.ullTotalPhys);
    used_memory_ =
        to_mega_bytes(memory_info_.ullTotalPhys - memory_info_.ullAvailPhys);
    used_memory_percentage_ = (double)used_memory_ / (double)total_memory_;
  }

 private:
  MEMORYSTATUSEX memory_info_;
  int64_t total_memory_{0};
  int64_t used_memory_{0};
  double used_memory_percentage_{0};
};

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
              }) | flex,
              vbox({separator(), separator()}),
              vbox({
                  hbox(text("Tasks: ") | color(Color::Cyan3),
                       text("290 total, 5 running")),
                  hbox(text("Size: ") | color(Color::Cyan3),
                       text(total_mem_stream.str()), text("GB")),
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

  SYSTEM_INFO siSysInfo;

  // Copy the hardware information to the SYSTEM_INFO structure.

  GetSystemInfo(&siSysInfo);

  // Display the contents of the SYSTEM_INFO structure.

  printf("Hardware information: \n");
  printf("  OEM ID: %u\n", siSysInfo.dwOemId);
  printf("  Number of processors: %u\n", siSysInfo.dwNumberOfProcessors);
  printf("  Page size: %u\n", siSysInfo.dwPageSize);
  printf("  Processor type: %u\n", siSysInfo.dwProcessorType);
  printf("  Minimum application address: %lx\n",
         siSysInfo.lpMinimumApplicationAddress);
  printf("  Maximum application address: %lx\n",
         siSysInfo.lpMaximumApplicationAddress);
  printf("  Active processor mask: %u\n", siSysInfo.dwActiveProcessorMask);
  printf("  Uptime: %d\n", GetTickCount64());


  return EXIT_SUCCESS;
}
