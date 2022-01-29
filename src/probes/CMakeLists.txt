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
	"memory_usage_probe.cpp"
	"system_info_probe.cpp"
	"system_times_probe.cpp" 
	"process_list_probe.cpp" 
	"../../include/probes/memory_usage_probe.h"
	"../../include/probes/system_info_probe.h" 
	"../../include/probes/system_times_probe.h" 
	"../../include/probes/process_list_probe.h")
target_link_libraries(probes PRIVATE WIL::WIL)
target_include_directories(probes
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gen
)

set_target_properties(probes PROPERTIES 
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)
