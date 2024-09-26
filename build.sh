#!/bin/bash

BUILD_DIR=".build"
EXECUTABLE="Sandbox/Sandbox"
EXAMPLES="Examples/example.py"
TESTING="Examples/tester.py"

function detect_platform() {
    case "$OSTYPE" in
        linux*)   platform="linux" ;;
        darwin*)  platform="macos" ;;
        msys* | cygwin* | win32*) platform="windows" ;;
        *)        platform="unknown" ;;
    esac
    echo "Detected platform: $platform"
}

function activate_venv() {
    if [ "$platform" == "windows" ]; then
        # Activate virtual environment on Windows
        .venv/Scripts/activate
    else
        # Activate virtual environment on Linux/macOS
        source .venv/bin/activate
    fi
}

function build() {
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    if [ "$platform" == "windows" ]; then
        cmake -G "Ninja" ..
    else
        cmake -G Ninja ..
    fi
    ninja
    cd ..
}

function run() {
    if [ "$platform" == "windows" ]; then
        ./$BUILD_DIR/$EXECUTABLE.exe
    else
        ./$BUILD_DIR/$EXECUTABLE
    fi
    echo "Running on platform: $platform"
}

function examples() {
    python3 $EXAMPLES
}

function tests() {
    echo "Running tests..."
    echo "Platform: $platform"
    echo "Checking Python version..."
    python3 --version
    
    echo "Verifying installed Python packages..."
    python3 -m pip list

    activate_venv

    echo "Executing Python tests from $TESTING"
    python3 $TESTING

    echo "Tests finished."
}

function clean() {
    rm -rf $BUILD_DIR
}

# Detect the platform first
detect_platform

if [ "$1" == "build" ]; then
    build
elif [ "$1" == "run" ]; then
    build
    run
elif [ "$1" == "examples" ]; then
    build
    examples
elif [ "$1" == "tests" ]; then
    build
    tests
elif [ "$1" == "clean" ]; then
    clean
else
    echo "Usage: $0 {build|run|examples|tests|clean}"
fi
