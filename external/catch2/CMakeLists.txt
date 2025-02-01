cmake_minimum_required(VERSION 3.12)

project(catch2 LANGUAGES CXX)

# Add library target
add_library(catch2 STATIC src/catch2.cpp)

# Specify include directories for the library
target_include_directories(catch2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Set C++ standard
set_target_properties(catch2 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Add an alias for Catch2::Catch2WithMain to ensure compatibility with your tests
add_library(Catch2::Catch2WithMain ALIAS catch2)
