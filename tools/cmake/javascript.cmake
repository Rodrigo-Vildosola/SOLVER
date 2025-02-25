message(STATUS "Building for WebAssembly with Emscripten")

# Ensure JSON library is included
include_directories(${NLOHMANN_JSON_DIR})

# Add include directories for the WASM target (so headers like compiler.h, pch.h, etc. are found)
include_directories("${CMAKE_SOURCE_DIR}/${HEADERS_DIR}")

# Locate JavaScript binding sources (written with Embind)
file(GLOB_RECURSE JS_BINDING_SRC "${CMAKE_SOURCE_DIR}/bindings/javascript/*.cpp")

# Build an executable target (Emscripten converts this to a .js loader + .wasm)
add_executable(_${LIB_NAME} ${LIB_SRC} ${JS_BINDING_SRC})

# Set properties so that Emscripten outputs a .js file and proper linker flags
set_target_properties(_${LIB_NAME} PROPERTIES 
    SUFFIX ".js"
    LINK_FLAGS "-s MODULARIZE=1 -s EXPORT_NAME='createSolverModule' -s EXPORT_ES6=1 -O3 -s ALLOW_MEMORY_GROWTH=1 -lembind -fexceptions"
)
