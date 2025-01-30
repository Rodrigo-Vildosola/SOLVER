cmake_minimum_required(VERSION 3.12)
project(Autopysta LANGUAGES CXX)

# Enable compile commands for better IDE integration
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# Add a compile definition for the version
add_compile_definitions(AUTOPYSTA_VERSION="${VERSION_CONTENTS}")

file(GLOB_RECURSE AUTOPYSTA_SRC "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE AUTOPYSTA_HEADERS "${CMAKE_SOURCE_DIR}/include/*.h" "${CMAKE_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE BINDING_SRC "${CMAKE_CURRENT_SOURCE_DIR}/bindings/*.cpp")

# Add include directories (Python and project-specific headers)
include_directories(${Python3_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)
include_directories(${PYBIND11_DIR})  # Include pybind11 headers

add_library(autopysta STATIC ${AUTOPYSTA_SRC})

# Create a shared library for Python bindings
add_library(_autopysta SHARED ${BINDING_SRC})


target_link_libraries(_autopysta PRIVATE autopysta)
target_link_libraries(autopysta ${Python3_LIBRARIES})

# Rename the shared library to match the Python module name
set_target_properties(_autopysta PROPERTIES PREFIX "" SUFFIX ".so")

# Compiler-specific options (GCC/Clang/MSVC)
if (MSVC)
    target_compile_options(autopysta PRIVATE 
        /W3 /EHsc /WX- 
        /D_CRT_SECURE_NO_WARNINGS
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Use compatible flags for GCC and Clang
    target_compile_options(autopysta PRIVATE 
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

# Output the shared library to the build directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Add clang-format target
find_program(CLANG_FORMAT_EXECUTABLE clang-format)
if (CLANG_FORMAT_EXECUTABLE)
    add_custom_target(
        clang-format ALL
        COMMAND ${CLANG_FORMAT_EXECUTABLE} 
        -i 
        -style=file 
        ${AUTOPYSTA_SRC} ${AUTOPYSTA_HEADERS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting source and header files with clang-format"
        VERBATIM
    )
else()
    message(WARNING "clang-format not found. Skipping formatting.")
endif()

message(STATUS "Project configuration complete.")

