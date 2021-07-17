# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("sapeaob-benchmark" C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_compile_definitions(PROJECT_BASEDIR="${PROJECT_SOURCE_DIR}")

add_subdirectory(src)
add_subdirectory(vendor)

if (MSVC)
	target_compile_options(sapeaob-benchmark PRIVATE /W3)
endif()