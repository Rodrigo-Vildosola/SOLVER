#!/bin/bash

# Directories and file paths
BUILD_DIR=".build"
EXECUTABLE="Sandbox/Sandbox"
EXAMPLES_SCRIPT="Examples/example.py"
BENCHMARK_SCRIPT="Examples/benchmark.py"
TEST_SCRIPT="Examples/tester.py"
PYTHON_OUTPUT_DIR="Examples/solver"
SOLVER_PYTHON_LIB="_SOLVER_PYTHON.so"
SOLVER_PYTHON_SCRIPT="solver.py"

# Detect platform (only macOS or Linux are supported here)
function detect_platform() {
    case "$OSTYPE" in
        linux*)   platform="linux" ;;
        darwin*)  platform="macos" ;;
        *)        platform="unknown" ;;
    esac
    echo "Detected platform: $platform"
}

# Create and build the project using CMake and Ninja
function build() {
    echo "Starting build process..."
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    cmake -G Ninja ..
    ninja
    cd ..
    echo "Build completed."
}

# Copy the generated Python shared library and script to the output directory
function copy_python_lib() {
    echo "Copying Python libraries..."
    mkdir -p $PYTHON_OUTPUT_DIR
    
    cp $BUILD_DIR/Solver/${SOLVER_PYTHON_LIB} $PYTHON_OUTPUT_DIR/${SOLVER_PYTHON_LIB}
    cp $BUILD_DIR/Solver/${SOLVER_PYTHON_SCRIPT} $PYTHON_OUTPUT_DIR/${SOLVER_PYTHON_SCRIPT}

    echo "Python libraries copied to $PYTHON_OUTPUT_DIR."
}

# Run the built C++ executable
function run_executable() {
    echo "Running the executable..."
    ./$BUILD_DIR/$EXECUTABLE
    echo "Execution finished."
}

function run_benchmark() {
    echo "Running Python benchmark..."
    python3 $BENCHMARK_SCRIPT

}

# Run the Python examples script
function run_examples() {
    echo "Running Python examples..."
    python3 $EXAMPLES_SCRIPT
}

# Run the Python test script
function run_tests() {
    echo "Running Python tests..."
    python3 $TEST_SCRIPT
    echo "Tests finished."
}

# Clean the build directory
function clean() {
    echo "Cleaning build directory..."
    rm -rf $BUILD_DIR
    echo "Clean completed."
}

# Main command handler
function main() {
    detect_platform

    case "$1" in
        build)
            build
            copy_python_lib
            ;;
        run)
            build
            copy_python_lib
            run_executable
            ;;
        examples)
            build
            copy_python_lib
            run_examples
            ;;
        tests)
            run_tests
            ;;
        benchmark)
            run_benchmark
            ;;
        clean)
            clean
            ;;
        *)
            echo "Usage: $0 {build|run|examples|tests|clean}"
            ;;
    esac
}

# Run the main function with the provided arguments
main "$@"
