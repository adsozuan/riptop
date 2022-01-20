#pragma once

#include "../probes/system_info_probe.h"
#include "../probes/system_times_probe.h"
#include "../probes/process_list_probe.h"
#include "../probes/memory_usage_probe.h"

#include <vector>
#include <string>
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"

namespace riptop
{
    using namespace ftxui;

    void      RenderMainUi();
    void      RenderMainUi(ScreenInteractive* screen);
    Component MakeMainUi();
} // namespace riptop
