#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/receiver.hpp>

namespace riptop
{
    struct SystemInfoDynamicData
    {
        float       cpu_usage {0.0};
        float       memory_usage_percentage {0.0};
        float       page_memory_usage_percentage {0.0};
        int32_t     total_tasks_count {0};
        int32_t     runnning_tasks_count {0};
        std::string up_time;
    };

    struct SystemInfoStaticData
    {
        std::string processor_name {""};
        std::string computer_name {""};
        int64_t     total_memory {0};
    };

    using SystemInfoDataReceiver = ftxui::Receiver<SystemInfoDynamicData>;
    using SystemInfoDataSender = ftxui::Sender<SystemInfoDynamicData>;

    class SystemInfoComponent : public ftxui::ComponentBase
    {
      public:
        SystemInfoComponent(SystemInfoStaticData static_data, SystemInfoDataReceiver receiver);
        ftxui::Element Render() override;
        bool           OnEvent(ftxui::Event event) override;

      private:
        SystemInfoDataReceiver receiver_;
        SystemInfoDynamicData  dynamic_data_ = {};
        SystemInfoStaticData   static_data_  = {};
    };

} // namespace riptop
