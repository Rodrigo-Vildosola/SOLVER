cmake_minimum_required(VERSION 3.12)
project(${LIB_NAME} LANGUAGES CXX)

# Enable compile commands for better IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYBIND11_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/pybind11/include)

message(STATUS "Python3 found:")
message(STATUS "  Executable    : ${Python3_EXECUTABLE}")
message(STATUS "  Version       : ${Python3_VERSION}")
message(STATUS "  Include dirs  : ${Python3_INCLUDE_DIRS}")
message(STATUS "  Library       : ${Python3_LIBRARIES}")

# Read the VERSION file
file(READ "${CMAKE_SOURCE_DIR}/VERSION" VERSION_CONTENTS)
string(STRIP "${VERSION_CONTENTS}" VERSION_CONTENTS)
message(STATUS "Project Version: ${VERSION_CONTENTS}")

message(STATUS "LIB_NAME: ${LIB_NAME}")
message(STATUS "HEADERS_DIR: ${HEADERS_DIR}")
message(STATUS "MODULE_NAME: ${MODULE_NAME}")
message(STATUS "PREFER: ${PREFER_DOUBLE}")

# Add a compile definition for the version and module name
add_compile_definitions(LIB_VERSION="${VERSION_CONTENTS}")
add_compile_definitions(MODULE_NAME=${MODULE_NAME})

# Gather core library sources and headers
file(GLOB_RECURSE LIB_SRC "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE LIB_HEADERS "${CMAKE_SOURCE_DIR}/${HEADERS_DIR}/*.h" "${CMAKE_SOURCE_DIR}/${HEADERS_DIR}/*.hpp")

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    message(STATUS "Building for WebAssembly with Emscripten")
    # Locate JavaScript binding sources (written with Embind)
    file(GLOB_RECURSE JS_BINDING_SRC "${CMAKE_CURRENT_SOURCE_DIR}/bindings/javascript/*.cpp")
    # Build an executable target (Emscripten converts this to a .js loader + .wasm)
    add_executable(${LIB_NAME}_js ${LIB_SRC} ${JS_BINDING_SRC})
    # Set properties so that Emscripten outputs a .js file and proper linker flags
    set_target_properties(${LIB_NAME}_js PROPERTIES 
        SUFFIX ".js"
        LINK_FLAGS "-s MODULARIZE=1 -s EXPORT_NAME='SolverModule' -O3 -s ALLOW_MEMORY_GROWTH=1"
    )
else()
    message(STATUS "Building native Python bindings")
    # Locate Python binding sources (using Pybind11)
    file(GLOB_RECURSE BINDING_SRC "${CMAKE_CURRENT_SOURCE_DIR}/bindings/python/*.cpp")
    # Build the static core library
    add_library(${LIB_NAME} STATIC ${LIB_SRC})
    # Create a shared library for Python bindings
    add_library(${MODULE_NAME} SHARED ${BINDING_SRC})
    target_link_libraries(${MODULE_NAME} PRIVATE ${LIB_NAME})
    target_link_libraries(${LIB_NAME} ${Python3_LIBRARIES})
    # Rename the shared library to match the Python module name
    set_target_properties(${MODULE_NAME} PROPERTIES PREFIX "" SUFFIX ".so")
    # Include directories (using paths passed in as arguments)
    include_directories(${Python3_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/${HEADERS_DIR}")
    include_directories(${PYBIND11_DIR})
endif()

# Compiler-specific options (example for GCC/Clang/MSVC)
if (MSVC)
    target_compile_options(${LIB_NAME} PRIVATE 
        /W3 /EHsc /WX- 
        /D_CRT_SECURE_NO_WARNINGS
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${LIB_NAME} PRIVATE 
        -Wall -Wundef -Wpedantic -Wextra 
        -Wuninitialized -Winit-self 
        -Wno-missing-field-initializers
        -Wno-unknown-pragmas
    )
endif()

if (WIN32)
    message(STATUS "Building on Windows")
elseif(APPLE)
    message(STATUS "Building on macOS")
else()
    message(STATUS "Building on Linux")
endif()

# Output the built shared library to the build directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

message(STATUS "Project configuration complete.")
