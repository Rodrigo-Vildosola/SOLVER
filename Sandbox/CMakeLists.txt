cmake_minimum_required(VERSION 3.10)

project(Sandbox)

file(GLOB_RECURSE SOURCES
    "src/*.h"
    "src/*.cpp"
)

add_executable(Sandbox ${SOURCES})

target_include_directories(Sandbox PRIVATE ${CMAKE_SOURCE_DIR}/Solver/src)
target_link_libraries(Sandbox PRIVATE SOLVER_STATIC)
