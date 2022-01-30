
# services library
add_library(services
    "../../include/services/system_data_service.h" 
	"system_data_service.cpp")
target_link_libraries(services
  PRIVATE ui
  PRIVATE ftxui::dom
  PRIVATE ftxui::component 
)
target_include_directories(services
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gen
)
set_target_properties(services PROPERTIES 
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)
