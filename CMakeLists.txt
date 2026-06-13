cmake_minimum_required(VERSION 3.25)

project(meph CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Core library
add_library(core
    src/core/scheduler.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Testing with Catch2
enable_testing()

# Fetch Catch2
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.6.0
)

FetchContent_MakeAvailable(Catch2)

# Test executable
add_executable(test_core
    test/test_core.cpp
)

target_link_libraries(test_core PRIVATE core Catch2::Catch2WithMain)

add_test(NAME CoreTests COMMAND test_core)

