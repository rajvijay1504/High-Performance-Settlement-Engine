cmake_minimum_required(VERSION 3.14)
project(SettlementSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tell CMake where to find header files (The 'src' folder)
include_directories(src)

# CONFIGURATION
# UNCOMMENT these two lines to check for memory leaks (Run unit_tests after uncommenting)
# NOTE: Re-comment them for performance benchmarking, as ASan slows down execution.
# add_compile_options(-fsanitize=address -g)
# add_link_options(-fsanitize=address)

# Find Packages
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

# Benchmark Executable (Point to 'tests' folder)
add_executable(perf_test tests/perf_benchmark.cpp)
target_link_libraries(perf_test benchmark::benchmark)

# Unit Test Executable (Point to 'tests' folder)
add_executable(unit_tests tests/real_tests.cpp)
target_link_libraries(unit_tests GTest::gtest GTest::gtest_main)