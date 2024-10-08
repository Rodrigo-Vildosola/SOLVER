cmake_minimum_required(VERSION 3.12)
project(SOLVER LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set CMake policies to handle SWIG and modern Python modules
cmake_policy(SET CMP0078 NEW)  # SWIG generates standard target names
cmake_policy(SET CMP0086 NEW)  # SWIG honors SWIG_MODULE_NAME via -module flag
cmake_policy(SET CMP0148 NEW)  # Use modern Python find modules

# Find SWIG and Python3 packages
find_package(SWIG REQUIRED COMPONENTS python)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
include("${SWIG_USE_FILE}")

# Display SWIG information
if(SWIG_FOUND)
  message(STATUS "SWIG found: ${SWIG_EXECUTABLE} (version: ${SWIG_VERSION})")
endif()

if(SWIG_python_FOUND)
  message(STATUS "SWIG Python bindings will be generated")
endif()

# Include directories for Python3 and the project
include_directories(${Python3_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src)

# List of C++ source files for the SOLVER library
file(GLOB SOLVER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Verify if source files were found (debugging output)
message(STATUS "SOLVER_SRC: ${SOLVER_SRC}")

# Add precompiled header support
set(PCH_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h")

# Build the SOLVER static C++ library
add_library(SOLVER_STATIC STATIC ${SOLVER_SRC})

# Add include directories for the static library
target_include_directories(SOLVER_STATIC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Precompiled header setup for SOLVER_STATIC
target_precompile_headers(SOLVER_STATIC PUBLIC ${PCH_HEADER})

# Set properties for the SWIG interface file
set_source_files_properties(solver.i PROPERTIES CPLUSPLUS ON)

# Generate the SWIG module (Python bindings) and compile the C++ code
swig_add_library(SOLVER_PYTHON
    TYPE MODULE
    LANGUAGE python
    SOURCES solver.i ${SOLVER_SRC}
)

# Link the SWIG module against the Python libraries
target_link_libraries(SOLVER_PYTHON ${Python3_LIBRARIES})

# Add include directories for the SWIG module
target_include_directories(SOLVER_PYTHON PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Precompiled header setup for SWIG module
target_precompile_headers(SOLVER_PYTHON PUBLIC ${PCH_HEADER})

# Add the Doxygen flag for SWIG
set(SWIG_FLAGS "-Wall" "-doxygen")

if (MSVC)
    target_compile_options(SOLVER_PYTHON PRIVATE 
        /W3 /EHsc /WX- 
        /D_CRT_SECURE_NO_WARNINGS
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Use compatible flags for GCC and Clang
    target_compile_options(SOLVER_PYTHON PRIVATE 
        -Wall -Wundef -Wpedantic -Wextra 
        -Wuninitialized -Winit-self 
        -Wno-missing-field-initializers
    )
endif()

# Add SWIG specific compile options
set_property(TARGET SOLVER_PYTHON PROPERTY SWIG_COMPILE_OPTIONS ${SWIG_FLAGS})

# Ensure the static library is built before the SWIG Python module
add_dependencies(SOLVER_PYTHON SOLVER_STATIC)

# Set output directory for the Python module
set(PYTHON_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Examples/solver")

# Determine platform-specific shared library extension
if(WIN32)
    set(SHARED_LIB_EXT "pyd")
elseif(APPLE)
    set(SHARED_LIB_EXT "so")
else()
    set(SHARED_LIB_EXT "so")
endif()
