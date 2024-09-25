#!/bin/bash

BUILD_DIR=".build"
EXECUTABLE="Sandbox/Sandbox"
EXAMPLES="Examples/tester.py"

function build() {
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    cmake -G Ninja ..
    ninja
    cd ..
}

function run() {
    ./$BUILD_DIR/$EXECUTABLE
}

function examples() {
    python3 $EXAMPLES
}

function clean() {
    rm -rf $BUILD_DIR
}

if [ "$1" == "build" ]; then
    build
elif [ "$1" == "run" ]; then
    build
    run
elif [ "$1" == "examples" ]; then
    build
    examples
elif [ "$1" == "clean" ]; then
    clean
else
    echo "Usage: $0 {build|run|clean}"
fi
