
# utils library
add_library(utils 
	"utils.cpp" 
	"../../include/utils/utils.h")
set_target_properties(utils PROPERTIES 
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	)

