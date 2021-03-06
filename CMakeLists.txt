cmake_minimum_required(VERSION 3.18)
project(RISC-V-SIM
        VERSION 0.1.0 
        LANGUAGES CXX
        HOMEPAGE_URL "https://github.com/RISC-V-CSE511-ECE511/RISC-V-Simulator")

include(CTest)


#Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(fmt REQUIRED)


add_definitions(-DEXAMPLES="${CMAKE_CURRENT_SOURCE_DIR}/test/test_files/")

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# Gets the include directory of boost (Base Path)
find_path(BOOST_INCLUDE_DIR boost/test/unit_test.hpp)

# Adds the base path to the include_directories ( All boost headers can be used now)
include_directories(${BOOST_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

add_subdirectory(test)
add_subdirectory(executable)

if(DEFINED ENV{GENERATE_BENCHMARK})
    find_package(benchmark CONFIG REQUIRED)
    add_subdirectory(benchmark) 
endif()

