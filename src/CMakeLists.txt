
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


# probes library
add_library(probes 
	"probes/memory_usage_info.cpp"
	"probes/system_info.cpp"
	"probes/system_times.cpp" 
	"probes/process_list.cpp" 
	"../include/probes/memory_usage_info.h"
	"../include/probes/system_info.h" 
	"../include/probes/system_times.h" 
	"../include/probes/process_list.h")
set_target_properties(probes PROPERTIES 
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)

# utils library
add_library(utils 
	"utils/formatter.cpp" 
	"../include/utils/formatter.h")
set_target_properties(utils PROPERTIES 
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)

# ui library
add_library(ui 
	"ui/main_ui.cpp" 
	"../include/ui/main_ui.h")
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


target_link_libraries(riptop
  PRIVATE probes
  PRIVATE utils
  PRIVATE ui
)

install(TARGETS riptop probes utils ui RUNTIME DESTINATION "bin")
