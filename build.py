#!/usr/bin/env python3
import argparse
import subprocess
import shutil
import glob
import os
import sys
import sysconfig
from tools.parse_config import BuildConfig

# Load configuration
config = BuildConfig.load_from_file("build_config.json")


def run_command(cmd, cwd=None):
    """Run a command and exit on error."""
    print("Running:", " ".join(cmd))
    subprocess.check_call(cmd, cwd=cwd)


def clean():
    """Remove build directories and extra directories."""
    build_dirs = [config.build_directory, "dist"]
    for d in build_dirs:
        if os.path.exists(d):
            shutil.rmtree(d)
            print(f"Removed directory: {d}")

    for extra in config.extra_directories:
        target_dir = os.path.join(extra, config.library_name)
        if os.path.exists(target_dir):
            shutil.rmtree(target_dir)
            print(f"Removed extra directory: {target_dir}")

    print("Clean complete.")


def build_pybind():
    """
    Build the Python binding target using CMake.
    This command will run CMake (with your current configuration)
    and compile the C++ solver and its pybind11 bindings.
    """
    # Ensure build directory exists
    os.makedirs(config.build_directory, exist_ok=True)
    build_dir = os.path.abspath(config.build_directory)

    # Configure CMake for pybind (note: you can add extra flags if needed)
    cmake_args = [
        "cmake",
        "-DPython3_EXECUTABLE=" + sys.executable,
        "-DCMAKE_BUILD_TYPE=Release",
        f"-DLIB_NAME={config.library_name}",
        f"-DHEADERS_DIR={config.headers_directory}",
        f"-DMODULE_NAME=_{config.library_name}",
        os.path.abspath("."),
    ]
    run_command(cmake_args, cwd=build_dir)

    # Build the project
    # For Windows you might need additional parameters like "--config Release"
    build_args = ["cmake", "--build", ".", "--config", "Release"]
    run_command(build_args, cwd=build_dir)
    print("Python binding build complete.")


def build_wasm():
    """
    Build the WebAssembly target using Emscripten.
    This command assumes that emscripten (emcmake, emmake, etc.) is available
    in your PATH.
    """
    # Create a separate subdirectory for the wasm build
    wasm_build_dir = os.path.join(config.build_directory, "wasm")
    os.makedirs(wasm_build_dir, exist_ok=True)
    wasm_build_dir = os.path.abspath(wasm_build_dir)

    # Configure CMake with emscripten toolchain (using emcmake)
    cmake_args = [
        "emcmake", "cmake",
        "-DCMAKE_BUILD_TYPE=Release",
        f"-DLIB_NAME={config.library_name}",
        f"-DHEADERS_DIR={config.headers_directory}",
        f"-DMODULE_NAME=_{config.library_name}",
        os.path.abspath("."),
    ]
    run_command(cmake_args, cwd=wasm_build_dir)

    # Build the project
    build_args = ["cmake", "--build", ".", "--config", "Release", "--", "-j2"]
    run_command(build_args, cwd=wasm_build_dir)
    print("WebAssembly build complete.")


def run_tests():
    """
    Run tests using pytest.
    Assumes tests are located in the directory specified by config.tests_directory.
    """
    tests_dir = os.path.abspath(config.tests_directory or "tests")
    if not os.path.exists(tests_dir):
        print(f"Tests directory not found: {tests_dir}")
        sys.exit(1)
    run_command(["pytest", "-s", tests_dir])
    print("Tests complete.")


def format_code():
    """
    Format all C/C++ source and header files using clang-format.
    """
    clang_format = shutil.which("clang-format")
    if not clang_format:
        print("Error: clang-format not found in PATH.")
        sys.exit(1)

    # Gather source and header files
    source_files = glob.glob(f"{config.source_directory}/**/*.cpp", recursive=True) + \
                   glob.glob(f"{config.source_directory}/**/*.c", recursive=True)
    header_files = glob.glob(f"{config.headers_directory}/**/*.h", recursive=True) + \
                   glob.glob(f"{config.headers_directory}/**/*.hpp", recursive=True)
    files_to_format = source_files + header_files

    if not files_to_format:
        print("No source/header files found to format.")
        return

    for file in files_to_format:
        print(f"Formatting: {file}")
        run_command([clang_format, "-i", file])
    print("Code formatting complete.")


def main():
    parser = argparse.ArgumentParser(
        description="Custom build system for the solver library."
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    # Define subcommands
    parser_clean = subparsers.add_parser("clean", help="Clean build directories.")
    parser_build_py = subparsers.add_parser("build_pybind", help="Build Python bindings.")
    parser_build_w = subparsers.add_parser("build_wasm", help="Build WebAssembly target.")
    parser_test = subparsers.add_parser("test", help="Run tests.")
    parser_format = subparsers.add_parser("format", help="Format source code.")

    args = parser.parse_args()

    if args.command == "clean":
        clean()
    elif args.command == "build_pybind":
        build_pybind()
    elif args.command == "build_wasm":
        build_wasm()
    elif args.command == "test":
        run_tests()
    elif args.command == "format":
        format_code()
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
