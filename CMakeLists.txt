cmake_minimum_required(VERSION 3.12)
project(mem)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MEM_BUILD_EXAMPLES "Build documentation examples" OFF)
option(MEM_BUILD_TESTS "Build documentation tests" OFF)

add_library(Mem INTERFACE)
target_include_directories(Mem
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

add_library(JED::mem ALIAS Mem)

if(MEM_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

if(MEM_BUILD_TESTS)
	add_subdirectory(tests)
endif()