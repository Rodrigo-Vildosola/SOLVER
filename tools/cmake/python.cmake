message(STATUS "Building native Python bindings")

message(STATUS "Python3 found:")
message(STATUS "  Executable    : ${Python3_EXECUTABLE}")
message(STATUS "  Version       : ${Python3_VERSION}")
message(STATUS "  Include dirs  : ${Python3_INCLUDE_DIRS}")
message(STATUS "  Library       : ${Python3_LIBRARIES}")

set(PYBIND11_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/pybind11/include)

# Locate Python binding sources (using Pybind11)
file(GLOB_RECURSE BINDING_SRC "${CMAKE_SOURCE_DIR}/bindings/python/*.cpp")

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
