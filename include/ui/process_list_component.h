#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/receiver.hpp>
#include <ftxui/dom/elements.hpp>

#include <format>

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
    using ProcessSender   = ftxui::Sender<std::vector<ProcessInfo>>;

    class ProcessListComponent : public ftxui::ComponentBase
    {
      public:
        ProcessListComponent(ProcessReceiver receiver);
        ftxui::Element RenderProcesses();
        bool           OnEvent(ftxui::Event event) override;

        int selected() { return selected_; }

      private:
        void                      SortProcessList(std::vector<ProcessInfo>* processes_to_sort);
        std::vector<std::wstring> Format(const std::vector<ProcessInfo>& processes);
        void                      HandleNavigation(const ftxui::Event& event);
        void                      SelectColumnSorting(const ftxui::Event& event);
        void                      HandleSearchActivation(const ftxui::Event& event);
        void                      HandleSearchInput(const ftxui::Event& event);
        void                      Search();

        ProcessReceiver           receiver_;
        std::vector<std::wstring> current_processes_;
        int                       selected_ {0};
        size_t                    process_count_ {0};
        ProcessSorting            sorting_ {};
        bool                      sort_order_ascending_ {};
        bool                      search_active_ {};
        std::wstring              search_string_ {};
    };

    class ColumnHeader
    {
      public:
        ColumnHeader() = delete;
        ColumnHeader(std::string name, ProcessSorting sorting, ProcessSorting selected_sorting,
                     bool sort_order_ascending, std::string key, int16_t width);

        std::string&   name() { return name_; }
        ProcessSorting sorting() { return sorting_; }
        std::string    key() { return key_; }
        int16_t        width() { return width_; }

      private:
        std::string    name_;
        ProcessSorting sorting_;
        std::string    key_;
        int16_t        width_;
    };
} // namespace riptop