cmake_minimum_required(VERSION 3.0.0)
project(MIPSAssembler)

file(GLOB_RECURSE PROJECT_SOURCES src/*.cc)
file(GLOB_RECURSE PROJECT_HEADERS include/*.h)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(assembler ${PROJECT_SOURCES} ${PROJECT_HEADERS})

target_include_directories(assembler PUBLIC include)

