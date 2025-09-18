cmake_minimum_required(VERSION 3.20)
project(mm_lab LANGUAGES C CXX)

# --- Apple Silicon defaults ---
if(APPLE)
  # Let you build universal later if you want, but default to arm64 for M1/M2.
  set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "" FORCE)
  # Prefer Homebrew’s default on Apple Silicon for pkg-config etc, just in case:
  list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Warnings that are strict but not masochistic
if (MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wno-sign-conversion)
endif()

include(FetchContent)

# --- fmt (header+tiny lib, best-in-class formatting) ---
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        11.0.2
)
FetchContent_MakeAvailable(fmt)

# --- nlohmann/json (header-only JSON) ---
FetchContent_Declare(nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# --- fast-cpp-csv-parser (header-only, no deps, fast) ---
FetchContent_Declare(fastcsv
  GIT_REPOSITORY https://github.com/ben-strasser/fast-cpp-csv-parser.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(fastcsv)

# --- NATS C client (official) ---
# We use the official C client and wrap it in a tiny RAII shim on the C++ side.
# This avoids relying on unmaintained third-party C++ bindings.
FetchContent_Declare(natsc
  GIT_REPOSITORY https://github.com/nats-io/nats.c.git
  GIT_TAG        v3.9.2
)
set(NATS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(NATS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# TLS optional on Day 1 (off keeps OpenSSL out for a simpler first build).
set(NATS_BUILD_WITH_TLS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(natsc)

# --- Targets ---
add_executable(csv_pub
  src/csv_pub.cpp
)

target_include_directories(csv_pub PRIVATE
  ${fastcsv_SOURCE_DIR}
  ${natsc_SOURCE_DIR}/src
  ${natsc_BINARY_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(csv_pub
  PRIVATE
    fmt::fmt
    nlohmann_json::nlohmann_json
    nats_static              # from nats.c
)

# On macOS, ensure we can find the static lib at runtime if we switch to shared later.
if(APPLE)
  set_target_properties(csv_pub PROPERTIES
    BUILD_RPATH "@loader_path"
    INSTALL_RPATH "@loader_path"
  )
endif()
