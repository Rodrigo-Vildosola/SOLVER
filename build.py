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
from tools.generate_stub import generate_stub  # Your stub generation function
from tools.mkdoc import mkdoc                # Your docstrings generator
from tools.logger import cli_print          # Your logging utility

class BuildFramework:
    def __init__(self, config: BuildConfig):
        self.config = config
        self.build_temp = os.path.abspath(config.build_directory)
        self.root_dir = os.path.abspath(config.root_directory)
        self.include_dir = os.path.abspath(config.headers_directory)
    
    def check_cmake(self):
        try:
            subprocess.check_call(["cmake", "--version"])
        except OSError as e:
            raise RuntimeError("CMake must be installed and on PATH.") from e
    
    def build_docstrings(self, build_temp: str):
        """Generates a docstrings file from header files using mkdoc."""
        headers = glob.glob(os.path.join(self.include_dir, "**/*.h"), recursive=True)
        headers += glob.glob(os.path.join(self.include_dir, "**/*.hpp"), recursive=True)
        headers = [h for h in headers if not h.endswith(self.config.docstrings_file)]
        output_path = os.path.join(self.include_dir, self.config.docstrings_file)
        cli_print("Generating docstrings...", level="info")
        mkdoc(headers, width=80, output=output_path, build_dir=build_temp)
        cli_print("Docstrings generated.", level="info")
    
    def initialize_environment(self):
        """
        Prepares the build environment: creates build directories and copies the VERSION file.
        Returns (py_version, build_temp, lib_temp).
        """
        py_version = f"{sys.version_info.major}.{sys.version_info.minor}"
        os.makedirs(self.build_temp, exist_ok=True)
        lib_temp = os.path.join(self.build_temp, self.config.library_name)
        os.makedirs(lib_temp, exist_ok=True)
        version_file = os.path.abspath(self.config.version_file)
        if os.path.exists(version_file):
            dest_version_file = os.path.join(lib_temp, "VERSION")
            copyfile(version_file, dest_version_file)
            cli_print(f"Copied VERSION file to {lib_temp}", level="info")
        else:
            cli_print(f"WARNING: VERSION file not found at {version_file}", level="warning")
        return py_version, self.build_temp, lib_temp
    
    def configure_cmake(self, py_version: str, target: str = "python"):
        python_executable = sys.executable
        python_include_dir = sysconfig.get_path('include')
        # Determine extension for the shared library.
        if platform.system() == "Windows":
            ext_file_ext = ".pyd"
        elif platform.system() == "Darwin":
            ext_file_ext = ".dylib"
        else:
            ext_file_ext = ".so"
        
        # Get Python library path.
        if platform.system() == "Windows":
            python_library = os.path.join(sysconfig.get_config_var('LIBDIR'),
                                          f"python{py_version.replace('.', '')}.lib")
        elif platform.system() == "Darwin":
            python_library = os.path.join(sysconfig.get_config_var('LIBDIR'),
                                          f"libpython{py_version}.dylib")
        else:
            python_library = os.path.join(sysconfig.get_config_var('LIBDIR'),
                                          f"libpython{py_version}.so")
        
        # Choose the CMake command and generator.
        if target == "wasm":
            cmake_cmd = "emcmake"
            generator = None
        else:
            cmake_cmd = "cmake"
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
            "-DCMAKE_BUILD_TYPE=Release"
        ]
        if target != "wasm" and python_library:
            cmake_args.append(f"-DPython3_LIBRARIES={python_library}")
        if generator:
            cmake_args.append(f"-G{generator}")
        
        cmake_args.append(f"-DLIB_NAME={self.config.library_name}")
        cmake_args.append(f"-DHEADERS_DIR={self.config.headers_directory}")
        cmake_args.append(f"-DMODULE_NAME=_{self.config.library_name}")
        
        cli_print(f"Configuring CMake for target '{target}'...", level="info")
        cmd = [cmake_cmd, self.root_dir] + cmake_args
        subprocess.check_call(cmd, cwd=self.build_temp)
        cli_print("CMake configuration completed.", level="info")
        return ext_file_ext
    
    def build_cmake(self, target: str = "python"):
        """Invokes the CMake build."""
        if platform.system() == "Windows":
            build_args = ["--config", "Release", "--", "/m"]
        else:
            build_args = ["--config", "Release", "--", "-j2"]
        cli_print("Building with CMake...", level="info")
        subprocess.check_call(["cmake", "--build", "."] + build_args, cwd=self.build_temp)
        cli_print("CMake build completed successfully.", level="info")
    
    def find_and_move_shared_object(self, ext_file_ext: str, lib_temp: str):
        """
        Finds the compiled shared object in the build directory and moves it to lib_temp.
        Returns the filename of the shared object.
        """
        module_prefix = f"_{self.config.library_name}"
        compiled_file = next(
            (f for f in os.listdir(self.build_temp)
             if f.startswith(module_prefix) and f.endswith(ext_file_ext)),
            None
        )
        if not compiled_file:
            compiled_file = next(
                (f for f in os.listdir(self.build_temp)
                 if f.startswith(module_prefix + ".") and (
                     f.endswith(".so") or f.endswith(".pyd") or f.endswith(".dll")
                 )),
                None
            )
        if not compiled_file:
            raise RuntimeError("No extension file found in the build directory.")
        src = os.path.join(self.build_temp, compiled_file)
        dst = os.path.join(lib_temp, compiled_file)
        os.rename(src, dst)
        cli_print(f"Moved shared object '{compiled_file}' to {lib_temp}", level="info")
        return compiled_file
    
    def generate_init_file(self, lib_temp: str, compiled_file: str):
        """Generates an __init__.py file to expose the module."""
        init_path = os.path.join(lib_temp, "__init__.py")
        base_module_name = os.path.splitext(compiled_file)[0]
        with open(init_path, "w") as f:
            f.write(f"from .{base_module_name} import *\n")
        print(f"-- Generated __init__.py at {init_path}")
    
    def generate_pyi_file(self, lib_temp: str):
        """Generates a .pyi stub file using generate_stub."""
        module_prefix = f"_{self.config.library_name}"
        pyi_path = os.path.join(lib_temp, f"{module_prefix}.pyi")
        generate_stub(module_prefix, pyi_path, lib_temp)
        print(f"-- Generated .pyi file at {pyi_path}")
    
    def copy_generated_files(self, destinations: list):
        """
        Copies the generated files from the build subdirectory to the specified destination directories.
        """
        source_dir = os.path.join(self.build_temp, self.config.library_name)
        if not os.path.exists(source_dir):
            cli_print(f"Source directory '{source_dir}' does not exist.", level="error")
            raise RuntimeError(f"Source directory '{source_dir}' does not exist.")
        for dest in destinations:
            dest = os.path.abspath(dest)
            dest_path = os.path.join(dest, self.config.library_name)
            os.makedirs(dest_path, exist_ok=True)
            for file in os.listdir(source_dir):
                src_file = os.path.join(source_dir, file)
                dst_file = os.path.join(dest_path, file)
                copyfile(src_file, dst_file)
            print(f"-- Copied generated files to {dest_path}")
    
    def clean(self):
        """
        Removes the build directory, 'dist', and any extra directories.
        """
        dirs_to_clean = [self.config.build_directory, "dist"]
        for d in dirs_to_clean:
            if os.path.exists(d):
                shutil.rmtree(d)
                cli_print(f"Removed directory: {d}", level="info")
        for extra in self.config.extra_directories:
            target_dir = os.path.join(extra, self.config.library_name)
            if os.path.exists(target_dir):
                shutil.rmtree(target_dir)
                cli_print(f"Removed extra directory: {target_dir}", level="info")
        cli_print("Clean complete.", level="info")
    
    def run_build(self, target: str = "python"):
        """
        Runs the entire build process for the given target ('python' or 'wasm').
        For the Python target, it performs extra steps to generate __init__.py and .pyi files.
        """
        self.check_cmake()
        self.build_docstrings(self.build_temp)
        py_version, build_temp, lib_temp = self.initialize_environment()
        ext_file_ext = self.configure_cmake(py_version, target)
        self.build_cmake(target)
        
        if target == "python":
            compiled_file = self.find_and_move_shared_object(ext_file_ext, lib_temp)
            self.generate_init_file(lib_temp, compiled_file)
            self.generate_pyi_file(lib_temp)
            destinations = [os.path.abspath(d) for d in self.config.extra_directories]
            self.copy_generated_files(destinations)
        elif target == "wasm":
            cli_print("WASM target build complete. WebAssembly artifacts are available in the build directory.", level="info")
        
        cli_print("Build process completed successfully.", level="success")


def main():
    import argparse
    parser = argparse.ArgumentParser(
        description="Build framework for C++ libraries with Python and JS (WebAssembly) bindings."
    )
    subparsers = parser.add_subparsers(dest="command", required=True)
    
    # Clean subcommand.
    parser_clean = subparsers.add_parser("clean", help="Clean build directories.")
    
    # Build subcommand with target option.
    parser_build = subparsers.add_parser("build", help="Build the project.")
    parser_build.add_argument("--target", choices=["python", "wasm"], default="python",
                              help="Specify target build: 'python' for Pybind, 'wasm' for WebAssembly.")
    
    args = parser.parse_args()
    config = BuildConfig.load_from_file("build_config.json")
    bf = BuildFramework(config)
    
    if args.command == "clean":
        bf.clean()
    elif args.command == "build":
        bf.run_build(target=args.target)
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
