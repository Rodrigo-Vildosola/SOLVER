cmake_minimum_required(VERSION 3.12)
project(${LIB_NAME} LANGUAGES CXX)

# Enable compile commands for better IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NLOHMANN_JSON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/nlohmann/include)

# Read the VERSION file
file(READ "${CMAKE_SOURCE_DIR}/VERSION" VERSION_CONTENTS)
string(STRIP "${VERSION_CONTENTS}" VERSION_CONTENTS)
message(STATUS "Project Version: ${VERSION_CONTENTS}")

message(STATUS "LIB_NAME: ${LIB_NAME}")
message(STATUS "HEADERS_DIR: ${HEADERS_DIR}")
message(STATUS "MODULE_NAME: ${MODULE_NAME}")

# Add a compile definition for the version and module name
add_compile_definitions(LIB_VERSION="${VERSION_CONTENTS}")
add_compile_definitions(MODULE_NAME=${MODULE_NAME})

# Gather core library sources and headers
file(GLOB_RECURSE LIB_SRC "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE LIB_HEADERS "${CMAKE_SOURCE_DIR}/${HEADERS_DIR}/*.h" "${CMAKE_SOURCE_DIR}/${HEADERS_DIR}/*.hpp")

include_directories(${NLOHMANN_JSON_DIR})

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    include(${CMAKE_SOURCE_DIR}/tools/cmake/javascript.cmake)
else()
    include(${CMAKE_SOURCE_DIR}/tools/cmake/python.cmake)
endif()


# Compiler-specific options (example for GCC/Clang/MSVC)
# if (MSVC)
#     target_compile_options(${LIB_NAME} PRIVATE 
#         /W3 /EHsc /WX- 
#         /D_CRT_SECURE_NO_WARNINGS
#     )
# elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#     target_compile_options(${LIB_NAME} PRIVATE 
#         -Wall -Wundef -Wpedantic -Wextra 
#         -Wuninitialized -Winit-self 
#         -Wno-missing-field-initializers
#         -Wno-unknown-pragmas
#     )
# endif()

if (WIN32)
    message(STATUS "Building on Windows")
elseif(APPLE)
    message(STATUS "Building on macOS")
else()
    message(STATUS "Building on Linux")
endif()

file(GLOB MAIN_SRC "${CMAKE_SOURCE_DIR}/src/main.cpp")
if(MAIN_SRC)
    add_executable(${LIB_NAME}_app ${MAIN_SRC})
    # Link the executable with the core static library if available.
    # In native builds, the core library target was created in python_bindings.cmake.
    if(TARGET ${LIB_NAME})
        target_link_libraries(${LIB_NAME}_app PRIVATE ${LIB_NAME})
    endif()
endif()

# Output the built shared library to the build directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

message(STATUS "Project configuration complete.")
