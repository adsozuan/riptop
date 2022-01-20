#pragma once

#include <cstdint>
#include <string>

namespace riptop
{
    class SystemInfoProbe
    {
      public:
        SystemInfoProbe();
        void        Update();
        std::string GetUptime() const;

        int64_t     cpu_core_count() const { return cpu_core_count_; }
        std::string processor_name() const { return processor_name_; }
        std::string computer_name() const { return computer_name_; }

      private:
        void UpdateCpuCount();
        void UpdateProcessorName();
        void UpdateComputerName();

      private:
        int64_t     cpu_core_count_ {0};
        std::string processor_name_;
        std::string computer_name_;
    };

} // namespace riptop
