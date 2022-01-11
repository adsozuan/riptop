

# --- Fetch Catch2 --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2
  GIT_TAG v2.13.8
)

 FetchContent_GetProperties(Catch2)
 if(NOT Catch2_POPULATED)
     FetchContent_Populate(Catch2)
     add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
 endif()
# ------------------------------------------------------------------------------


# tests
find_package(Catch2 REQUIRED)

include(CTest)

add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC Catch2::Catch2)

add_executable(riptop_test "test_utils.cpp")
target_link_libraries(riptop_test PRIVATE catch_main utils)



