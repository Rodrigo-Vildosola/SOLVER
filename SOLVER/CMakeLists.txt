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
# Include all .cpp files in the src/ and src/Core/ directories
file(GLOB SOLVER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/Core/*.cpp")

# Verify if source files were found (debugging output)
message(STATUS "SOLVER_SRC: ${SOLVER_SRC}")

# Build the SOLVER static C++ library
add_library(SOLVER_STATIC STATIC ${SOLVER_SRC})

# Add include directories for the static library
target_include_directories(SOLVER_STATIC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

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

# Add the necessary compile options to SWIG
target_compile_options(SOLVER_PYTHON PRIVATE 
    -Wall -Wundef -Wpedantic -Wextra 
    -Wuninitialized -Winit-self
    -Wno-error=unused-command-line-argument
)

# Ensure the static library is built before the SWIG Python module
add_dependencies(SOLVER_PYTHON SOLVER_STATIC)


set(PYTHON_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Examples/lib")

# Use correct paths for copying the generated files
add_custom_command(TARGET SOLVER_PYTHON POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_BINARY_DIR}/SOLVER/_SOLVER_PYTHON.so"
    "${PYTHON_OUTPUT_DIR}/_SOLVER_PYTHON.so"
)

add_custom_command(TARGET SOLVER_PYTHON POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_BINARY_DIR}/SOLVER/solver.py"
    "${PYTHON_OUTPUT_DIR}/solver.py"
)
