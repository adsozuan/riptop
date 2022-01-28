
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

# --- Fetch WIL --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(
  wil
  GIT_REPOSITORY https://github.com/microsoft/wil 
)

FetchContent_GetProperties(wil)
if(NOT wil_POPULATED)
  FetchContent_Populate(wil)
  add_subdirectory(${wil_SOURCE_DIR} ${wil_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------

# probes library
add_library(probes 
	"probes/memory_usage_probe.cpp"
	"probes/system_info_probe.cpp"
	"probes/system_times_probe.cpp" 
	"probes/process_list_probe.cpp" 
	"../include/probes/memory_usage_probe.h"
	"../include/probes/system_info_probe.h" 
	"../include/probes/system_times_probe.h" 
	"../include/probes/process_list_probe.h")
set_target_properties(probes PROPERTIES 
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)

target_link_libraries(probes PRIVATE WIL::WIL)

# utils library
add_library(utils 
	"utils/utils.cpp" 
	"../include/utils/utils.h")
set_target_properties(utils PROPERTIES 
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)

# ui library
add_library(ui 
	"../include/ui/main_component.h" 
	"../include/ui/process_list_component.h"
	"../include/ui/system_info_component.h"
	"ui/process_list_component.cpp" 
	"ui/main_component.cpp" 
	"ui/system_info_component.cpp")
target_link_libraries(ui
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component 
)
set_target_properties(ui PROPERTIES 
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)


add_executable(riptop "main.cpp")
set_target_properties(riptop PROPERTIES 
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_include_directories(riptop
  PRIVATE "../include"
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gen
)



target_link_libraries(riptop
  PRIVATE ftxui::screen
  PRIVATE ftxui::component
  PRIVATE probes
  PRIVATE utils
  PRIVATE ui
)

if (MSVC)
  target_compile_options(riptop PRIVATE "/wd4244")
  target_compile_options(riptop PRIVATE "/wd4267")
  target_compile_options(riptop PRIVATE "/wd4996")
  target_compile_options(riptop PRIVATE "/wd4305")
endif()

install(TARGETS riptop probes utils ui RUNTIME DESTINATION "bin")
