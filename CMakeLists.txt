cmake_minimum_required (VERSION 3.11)

set(CMAKE_CXX_STANDARD 20)


project(riptop
  LANGUAGES CXX
  VERSION 1.0.0
)

add_subdirectory(src)

option(ENABLE_TESTING "Enable Test Builds" ON)

if(ENABLE_TESTING)
  enable_testing()
  message("Building Tests. Be sure to check out test/constexpr_tests for constexpr testing")
  add_subdirectory(test)
endif()

