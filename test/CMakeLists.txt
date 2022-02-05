# --- Fetch Catch2 --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2
  GIT_TAG v2.13.8
)
FetchContent_MakeAvailable(Catch2)
# ------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)

add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC Catch2::Catch2)

add_executable(riptop_test "test_utils.cpp" "test_probes.cpp")
target_link_libraries(riptop_test PRIVATE catch_main utils)



