#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/receiver.hpp>

struct ProcessInfo
{
    int64_t     id;
    std::string user_name;
    int64_t     base_priority;
    double      percent_processor_time;
    uint64_t    used_memory;
    uint64_t    thread_count;
    uint64_t    disk_usage;
    uint64_t    up_time;
    std::string exe_name;
};

using ProcessReceiver = ftxui::Receiver<std::vector<ProcessInfo>>;

class ProcessListComponent : public ftxui::ComponentBase
{
  public:
    ProcessListComponent(ProcessReceiver receiver);
    ftxui::Element RenderProcesses();
    bool           OnEvent(ftxui::Event event) override;

    int selected() { return selected_; }

  private:
    void Format(const std::vector<ProcessInfo>& processes);

    ProcessReceiver          receiver_;
    std::vector<std::string> processes_;
    int                      selected_ {0};
};