#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/receiver.hpp>
#include <ftxui/dom/elements.hpp>

namespace riptop
{
    enum class ProcessSorting
    {
        CpuPercent,
        Memory,
        Pid,
        ProcessName,
        User,
        DiskUsage,
        UpTime,
        Priority,
        Thread,
    };

    enum class SortOrder
    {
        Ascending,
        Descending
    };

    struct ProcessInfo
    {
        int64_t     id {};
        std::string user_name {};
        int64_t     base_priority {};
        double      percent_processor_time {};
        uint64_t    used_memory {};
        uint64_t    thread_count {};
        uint64_t    disk_usage {};
        uint64_t    up_time {};
        std::string exe_name {};
    };

    using ProcessReceiver = ftxui::Receiver<std::vector<ProcessInfo>>;
    using ProcessSender = ftxui::Sender<std::vector<ProcessInfo>>;

    class ProcessListComponent : public ftxui::ComponentBase
    {
      public:
        ProcessListComponent(ProcessReceiver receiver);
        ftxui::Element RenderProcesses();
        bool           OnEvent(ftxui::Event event) override;

        int selected() { return selected_; }

      private:
        void SortProcessList(std::vector<ProcessInfo>* processes_to_sort, ProcessSorting sort_type, SortOrder sort_order);
        std::vector<std::wstring> Format(const std::vector<ProcessInfo>& processes);

        ProcessReceiver           receiver_;
        std::vector<std::wstring> current_processes_;
        int                       selected_ {0};
        size_t                    process_count_ {0};
        ProcessSorting            sorting_ {};
        SortOrder                 sort_order_ {};
    };
} // namespace riptop