include_directories("../include")

add_subdirectory(utils)
add_subdirectory(probes)
add_subdirectory(ui)
add_subdirectory(services)


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
  PRIVATE services
)

if (MSVC)
  target_compile_options(riptop PRIVATE "/wd4244")
  target_compile_options(riptop PRIVATE "/wd4267")
  target_compile_options(riptop PRIVATE "/wd4996")
  target_compile_options(riptop PRIVATE "/wd4305")
endif()

install(TARGETS riptop probes utils ui services RUNTIME DESTINATION "bin")
