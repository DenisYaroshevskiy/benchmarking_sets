cmake_minimum_required(VERSION 3.7)

SET(CMAKE_CXX_COMPILER clang++-mp-7.0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_subdirectory(third_party)

project(benchmarking_sets)

include_directories(./ ./third_party ./third_party/abseil-cpp/ ../srt-library)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -g)

add_executable(benchmarks)
target_sources(benchmarks PRIVATE
               #erase_by_one.cc
               #erase_bulk.cc
               #lookup_speed_out_of_cache.cc
               lookup_speed_in_cache.cc
               #insert_f_l_bench.cc
               #range_construction.cc
               google_benchmark_main.cc)
set_target_properties(benchmarks PROPERTIES CXX_STANDARD 17)
target_compile_options(benchmarks PRIVATE -O3)
target_link_libraries(benchmarks absl::flat_hash_set)
target_link_libraries(benchmarks benchmark)
