cmake_minimum_required (VERSION 3.11)

set(CMAKE_CXX_STANDARD 20)

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v0.11
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------

project(riptop
  LANGUAGES CXX
  VERSION 1.0.0
)

# probes library
add_library(probes 
	"src/probes/memory_usage_info.cpp"
	"src/probes/system_info.cpp"
	"include/probes/memory_usage_info.h"
	"include/probes/system_info.h" "include/probes/system_times.h" "src/probes/system_times.cpp")
set_target_properties(probes PROPERTIES LINKER_LANGUAGE CXX)

# utils library
add_library(utils 
	"src/utils/formatter.cpp" 
	"include/utils/formatter.h")
set_target_properties(utils PROPERTIES LINKER_LANGUAGE CXX)

# ui library
add_library(ui 
	"src/ui/main_ui.cpp" 
	"include/ui/main_ui.h")
target_link_libraries(ui
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
)
set_target_properties(ui PROPERTIES LINKER_LANGUAGE CXX)

add_executable(riptop src/main.cpp)

target_link_libraries(riptop
  PRIVATE probes
  PRIVATE utils
  PRIVATE ui
)

install(TARGETS riptop probes utils ui RUNTIME DESTINATION "bin")

option(ENABLE_TESTING "Enable Test Builds" ON)

if(ENABLE_TESTING)
  enable_testing()
  message("Building Tests. Be sure to check out test/constexpr_tests for constexpr testing")
  add_subdirectory(test)
endif()

