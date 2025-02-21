#!/usr/bin/env python3
import os
import sys
import platform
import subprocess
import sysconfig
import shutil
import glob
from shutil import copyfile
from tools.parse_config import BuildConfig
from tools.generate_stub import generate_stub  # your stub generation function
from tools.mkdoc import mkdoc                # your docstrings generator
from tools.logger import cli_print          # your logging utility

# Load configuration from build_config.json
config = BuildConfig.load_from_file("build_config.json")


def check_cmake():
    try:
        subprocess.check_call(["cmake", "--version"])
    except OSError:
        raise RuntimeError("CMake must be installed and on PATH.")


def initialize_build_environment(config, build_temp):
    """
    Prepares the build environment by ensuring the temporary and output directories exist
    and copies the VERSION file.
    """
    py_version = f"{sys.version_info.major}.{sys.version_info.minor}"
    build_temp = os.path.abspath(build_temp)
    lib_temp = os.path.join(build_temp, config.library_name)
    os.makedirs(build_temp, exist_ok=True)
    os.makedirs(lib_temp, exist_ok=True)

    version_file = os.path.abspath(config.version_file)
    if os.path.exists(version_file):
        dest_version_file = os.path.join(lib_temp, "VERSION")
        copyfile(version_file, dest_version_file)
        print(f"Copied VERSION file to {lib_temp}")
    else:
        print(f"WARNING: VERSION file not found at {version_file}")
    return py_version, build_temp, lib_temp


def configure_cmake(config, build_temp, py_version):
    """
    Configures the CMake project by passing the current Python executable,
    include directories, version, and other settings.
    """
    python_executable = sys.executable
    python_include_dir = sysconfig.get_path('include')

    # Determine the correct extension suffix for the shared library
    if platform.system() == "Windows":
        ext_file_ext = ".pyd"
    elif platform.system() == "Darwin":
        ext_file_ext = ".dylib"
    else:
        ext_file_ext = ".so"

    # Find Python library
    if platform.system() == "Windows":
        python_library = os.path.join(sysconfig.get_config_var('LIBDIR'),
                                      f"python{py_version.replace('.', '')}.lib")
    elif platform.system() == "Darwin":
        python_library = os.path.join(sysconfig.get_config_var('LIBDIR'),
                                      f"libpython{py_version}.dylib")
    else:
        python_library = os.path.join(sysconfig.get_config_var('LIBDIR'),
                                      f"libpython{py_version}.so")

    # Choose CMake generator
    if shutil.which("ninja"):
        generator = "Ninja"
    elif platform.system() == "Windows":
        generator = "Visual Studio 16 2019"
    else:
        generator = "Unix Makefiles"

    cmake_args = [
        f"-DPython3_EXECUTABLE={python_executable}",
        f"-DPython3_INCLUDE_DIRS={python_include_dir}",
        f"-DPython3_VERSION={py_version}",
        "-DCMAKE_BUILD_TYPE=Release",
        f"-G{generator}"
    ]
    if python_library:
        cmake_args.append(f"-DPython3_LIBRARIES={python_library}")

    cmake_args.append(f"-DLIB_NAME={config.library_name}")
    cmake_args.append(f"-DHEADERS_DIR={config.headers_directory}")
    cmake_args.append(f"-DMODULE_NAME=_{config.library_name}")

    root_dir = os.path.abspath(config.root_directory)
    cli_print("Configuring CMake...", level="info")
    subprocess.check_call(["cmake", root_dir] + cmake_args, cwd=build_temp)
    cli_print("CMake configuration completed.", level="info")
    return ext_file_ext


def build_cmake(build_temp):
    """
    Invokes CMake to build the project.
    """
    if platform.system() == "Windows":
        build_args = ["--config", "Release", "--", "/m"]
    else:
        build_args = ["--config", "Release", "--", "-j2"]

    cli_print("Building with CMake...", level="info")
    subprocess.check_call(["cmake", "--build", "."] + build_args, cwd=build_temp)
    cli_print("CMake build completed successfully.", level="info")


def find_and_move_shared_object(config, build_temp, lib_temp, ext_file_ext):
    """
    Searches the build_temp for the compiled shared object and moves it to lib_temp.
    """
    module_prefix = f"_{config.library_name}"
    compiled_file = next(
        (f for f in os.listdir(build_temp)
         if f.startswith(module_prefix) and f.endswith(ext_file_ext)),
        None
    )
    if not compiled_file:
        compiled_file = next(
            (f for f in os.listdir(build_temp)
             if f.startswith(module_prefix + ".") and (
                    f.endswith(".so") or f.endswith(".pyd") or f.endswith(".dll")
                )),
            None
        )
    if not compiled_file:
        raise RuntimeError("No extension file found in the build_temp directory.")

    src = os.path.join(build_temp, compiled_file)
    dst = os.path.join(lib_temp, compiled_file)
    os.rename(src, dst)
    return compiled_file


def generate_init_file(lib_temp, compiled_file, library_name):
    """
    Generates an __init__.py file in lib_temp to expose the module.
    """
    init_path = os.path.join(lib_temp, "__init__.py")
    base_module_name = os.path.splitext(compiled_file)[0]
    with open(init_path, "w") as f:
        f.write(f"from .{base_module_name} import *\n")
    print(f"-- Generated __init__.py at {init_path}")


def generate_pyi_file(lib_temp, library_name):
    """
    Generates a .pyi stub file using your generate_stub helper.
    """
    module_prefix = f"_{library_name}"
    pyi_path = os.path.join(lib_temp, f"{module_prefix}.pyi")
    generate_stub(module_prefix, pyi_path, lib_temp)
    print(f"-- Generated .pyi file: {pyi_path}")


def copy_generated_files(build_temp, config, destinations):
    """
    Copies generated files (such as the __init__.py, .pyi, VERSION, etc.) from the
    temporary build subdirectory to the specified destination directories.
    """
    source_dir = os.path.join(build_temp, config.library_name)
    if not os.path.exists(source_dir):
        cli_print(f"Source directory {source_dir} does not exist.", level="error")
        raise RuntimeError(f"Source directory {source_dir} does not exist.")

    for dest in destinations:
        dest_path = os.path.join(dest, config.library_name)
        os.makedirs(dest_path, exist_ok=True)
        for file in os.listdir(source_dir):
            src_file = os.path.join(source_dir, file)
            dst_file = os.path.join(dest_path, file)
            copyfile(src_file, dst_file)
        print(f"-- Copied generated files to {dest_path}")


def build_docstrings(config, include_dir, build_temp):
    """
    Runs the documentation generator (mkdoc) on the header files (excluding the
    docstrings file) and writes the output to the configured docstrings file.
    """
    headers = glob.glob(os.path.join(include_dir, "**/*.h"), recursive=True)
    headers += glob.glob(os.path.join(include_dir, "**/*.hpp"), recursive=True)
    headers = [h for h in headers if not h.endswith(config.docstrings_file)]
    output_path = os.path.join(include_dir, config.docstrings_file)
    mkdoc(headers, width=80, output=output_path, build_dir=build_temp)


def build_ext_custom():
    """
    Emulates the custom build_ext command:
      - Checks for CMake.
      - Builds docstrings.
      - Initializes the build environment.
      - Configures CMake (passing in Python executable, include dirs, etc.).
      - Builds the project.
      - Finds and moves the compiled shared object.
      - Generates __init__.py and .pyi stub files.
      - Copies generated files to target directories.
    """
    check_cmake()

    # Build docstrings from headers
    include_dir = config.headers_directory
    build_temp_dir = config.build_directory  # Use the configured build directory
    os.makedirs(build_temp_dir, exist_ok=True)
    build_docstrings(config, include_dir, build_temp_dir)

    # Initialize build environment
    py_version, build_temp, lib_temp = initialize_build_environment(config, build_temp_dir)

    # Configure CMake and capture the shared object extension
    ext_file_ext = configure_cmake(config, build_temp, py_version)

    # Build using CMake
    build_cmake(build_temp)

    # Find the shared object and move it into the library directory
    compiled_file = find_and_move_shared_object(config, build_temp, lib_temp, ext_file_ext)

    # Generate __init__.py and .pyi files in the library directory
    generate_init_file(lib_temp, compiled_file, config.library_name)
    generate_pyi_file(lib_temp, config.library_name)

    default_destinations = [os.path.abspath(d) for d in config.extra_directories]
    copy_generated_files(build_temp, config, default_destinations)

    cli_print("Custom build_ext process completed successfully.", level="success")


def clean():
    """
    Removes the configured build directory, 'dist', and any extra directories.
    """
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


def main():
    import argparse
    parser = argparse.ArgumentParser(
        description="Emulate Custom BuildExt process for the solver library."
    )
    parser.add_argument("--clean", action="store_true", help="Clean build directories before building.")
    args = parser.parse_args()

    if args.clean:
        print("Cleaning build directories...")
        clean()

    build_ext_custom()


if __name__ == "__main__":
    main()
