#include "../../include/ui/system_info_component.h"
#include "../../include/utils/utils.h"

#include "ftxui/dom/elements.hpp"
#include "ftxui/component/event.hpp"

#include <format>

SystemInfoComponent::SystemInfoComponent(SystemInfoStaticData static_data, SystemInfoDataReceiver receiver)
    : static_data_(static_data), receiver_(std::move(receiver))
{
}

ftxui::Element SystemInfoComponent::Render()
{
    using namespace ftxui;

    while (receiver_->HasPending())
    {
        receiver_->Receive(&dynamic_data_);
    }

    // clang-format off
    auto usage_gauge = [](std::string name, float value) {
        auto gauge_color = Color::Green;

        if (value > 0.5)
        {
            gauge_color = Color::Yellow;
        }
        if (value > 0.75)
        {
            gauge_color = Color::Red;
        }
        auto content = hbox(separatorEmpty(), text(name) | color(Color::Cyan3), text("["),
                            gauge(value) | color(gauge_color) | flex,
                            text(std::format("{:2.1f}", value * 100)) | color(Color::GrayDark),
                            text("%") | color(Color::GrayDark), text("]")) |
                       size(WIDTH, EQUAL, 35);
        return content;
    };

    auto usage_gauges_area = [&]() {
        return vbox({
            usage_gauge("CPU", dynamic_data_.cpu_usage),
            usage_gauge("MEM", dynamic_data_.memory_usage_percentage),
            usage_gauge("PGE", dynamic_data_.page_memory_usage_percentage),
        });
    };


    auto system_info_area = [&]() {
		auto tasks_number = std::format("{} total, {} runnings", dynamic_data_.total_tasks_count, dynamic_data_.runnning_tasks_count);
		auto total_memory = std::format("{} GB", riptop::to_mega_bytes(static_data_.total_memory) / 1000);
        return vbox({
            hbox(separatorEmpty(), text("Tasks: ") | color(Color::Cyan3), text(tasks_number)),  
            hbox(separatorEmpty(), text("Size: ") | color(Color::Cyan3), text(total_memory)),
            hbox(separatorEmpty(), text("Uptime: ") | color(Color::Cyan3), text(dynamic_data_.up_time)),
            hbox(separatorEmpty(), text("Proc: ") | color(Color::Cyan3), text(static_data_.processor_name)),
        });
    };

    auto global_usage = Renderer([&] {
        auto content = hbox({usage_gauges_area(), filler(), system_info_area()});
        return content;
    });

    return vbox(global_usage->Render());

    // clang-format on
}

bool SystemInfoComponent::OnEvent(ftxui::Event event) { return ftxui::ComponentBase::OnEvent(event); }