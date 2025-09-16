cmake_minimum_required(VERSION 3.20)
project(mm_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(MM_LAB_WARNINGS_AS_ERRORS "Treat warnings as errors" ON)
option(MM_LAB_BUILD_TESTS "Build tests" ON)

# Global warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnon-virtual-dtor)
  if (MM_LAB_WARNINGS_AS_ERRORS)
    add_compile_options(-Werror)
  endif()
endif()

# Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include dirs
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Apps
add_executable(mm_sim apps/mm_sim.cpp)
add_executable(replay_tool apps/tools/replay.cpp)

# Tests (simple)
if (MM_LAB_BUILD_TESTS)
  enable_testing()
  add_executable(smoke_test tests/smoke_test.cpp)
  add_test(NAME smoke COMMAND smoke_test)
endif()
