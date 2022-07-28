cmake_minimum_required (VERSION 3.5)

project (system)

set(CMAKE_C_STANDARD 11)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g")
set (source_dir "${PROJECT_SOURCE_DIR}/src/")
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

file (GLOB source_files "${source_dir}/*.c")
include_directories(${PROJECT_SOURCE_DIR}/include/)

add_executable (system ${source_files})
