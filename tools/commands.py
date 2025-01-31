from setuptools.command.build_ext import build_ext
from setuptools.command.build_py import build_py
from setuptools.command.build import build
from setuptools import Command
from .generate_stub import generate_stub
from .mkdoc import mkdoc
from .logger import cli_print
from .parse_config import BuildConfig
from shutil import copyfile
import os
import platform
import subprocess
import glob
import sysconfig
import sys
import shutil

class CustomBuild(build):
    def __init__(self, dist, config=None):
        self.config: BuildConfig = config
        super().__init__(dist)

    def run(self):
        self.run_command('build_ext')
        self.run_command('build_py')
        cli_print("Build process completed successfully.", level="success")


class CustomBuildExt(build_ext):
    def __init__(self, dist, config=None):
        self.config: BuildConfig = config
        super().__init__(dist)

    def initialize_options(self):
        super().initialize_options()
        self.include_dir = self.config.headers_directory

    def run(self):
        self.check_cmake()
        self.build_docstrings()
        for ext in self.extensions:
            self.build_extension()

    def build_extension(self):
        self.initialize_build_environment()
        self.run_cmake()
        so_file = self.find_and_move_shared_object()
        self.generate_init_file(so_file)
        self.generate_pyi_file()

        # -- Use config for destinations
        # Always place in self.build_lib plus any "extra_directories" from config
        default_destinations = [os.path.abspath(d) for d in self.config.extra_directories]
        destinations = [self.build_lib] + default_destinations
        self.copy_generated_files(self.build_temp, destinations)

    def build_docstrings(self):
        headers = [
            header for header in glob.glob(f"{self.include_dir}/**/*.h", recursive=True)
            if not header.endswith(self.config.docstrings_file)
        ]
        output_path = os.path.join(self.include_dir, self.config.docstrings_file)

        mkdoc(headers, width=80, output=output_path, build_dir=self.build_temp)

    def check_cmake(self):
        try:
            subprocess.check_call(["cmake", "--version"])
        except OSError:
            raise RuntimeError("CMake must be installed and on PATH.")

    def initialize_build_environment(self):
        self.py_version = f"{sys.version_info.major}.{sys.version_info.minor}"
        self.build_temp = os.path.abspath(self.build_temp)

        # Use the library name as subdir for compiled artifacts
        self.lib_temp = os.path.join(self.build_temp, self.config.library_name)
        os.makedirs(self.build_temp, exist_ok=True)
        os.makedirs(self.lib_temp, exist_ok=True)

        # Copy VERSION file (or the configured version_file)
        version_file = os.path.abspath(self.config.version_file)
        if os.path.exists(version_file):
            dest_version_file = os.path.join(self.lib_temp, "VERSION")
            copyfile(version_file, dest_version_file)
            print(f"Copied VERSION file to {self.lib_temp}")
        else:
            print("WARNING: VERSION file not found at expected path:", version_file)


    def run_cmake(self):
        self.configure_cmake()
        self.build_cmake()

    def configure_cmake(self):
        python_executable = sys.executable
        python_include_dir = sysconfig.get_path('include')

        # Determine extension
        if platform.system() == "Windows":
            self.ext_file_ext = ".pyd"
        elif platform.system() == "Darwin":
            self.ext_file_ext = ".dylib"
        else:
            self.ext_file_ext = ".so"

        # Find python library if needed
        python_library = self.get_python_library()
        generator = self.get_cmake_generator()

        cmake_args = [
            f"-DPython3_EXECUTABLE={python_executable}",
            f"-DPython3_INCLUDE_DIRS={python_include_dir}",
            f"-DPython3_VERSION={self.py_version}",
            "-DCMAKE_BUILD_TYPE=Release",
            f"-G{generator}"
        ]
        if python_library:
            cmake_args.append(f"-DPython3_LIBRARIES={python_library}")
        
        cmake_args.append(f"-DLIB_NAME={self.config.library_name}")
        cmake_args.append(f"-DHEADERS_DIR={self.config.headers_directory}")
        cmake_args.append(f"-DMODULE_NAME=_{self.config.library_name}")
        cmake_args.append(f"-DHEADERS_DIR={self.config.headers_directory}")

        # Source directory from config
        root_dir = os.path.abspath(self.config.root_directory)

        cli_print("Configuring CMake...", level="info")
        subprocess.check_call(["cmake", root_dir] + cmake_args, cwd=self.build_temp)
        cli_print("CMake configuration completed.", level="info")

    def build_cmake(self):
        build_args = []
        if platform.system() == "Windows":
            build_args += ["--config", "Release"]
            if "Visual Studio" in self.get_cmake_generator():
                build_args += ["--", "/m"]
        else:
            build_args += ["--config", "Release", "--", "-j2"]

        cli_print("Building with CMake...", level="info")
        subprocess.check_call(["cmake", "--build", "."] + build_args, cwd=self.build_temp)
        cli_print("CMake build completed successfully.", level="info")

    def get_python_library(self):
        if platform.system() == "Windows":
            return os.path.join(
                sysconfig.get_config_var('LIBDIR'),
                f"python{self.py_version.replace('.', '')}.lib"
            )
        elif platform.system() == "Darwin":
            return os.path.join(
                sysconfig.get_config_var('LIBDIR'),
                f"libpython{self.py_version}.dylib"
            )
        else:  # Linux or Unix
            return os.path.join(
                sysconfig.get_config_var('LIBDIR'),
                f"libpython{self.py_version}.so"
            )

    def get_cmake_generator(self):
        if shutil.which("ninja"):
            return "Ninja"
        elif platform.system() == "Windows":
            return "Visual Studio 16 2019"
        else:
            return "Unix Makefiles"

    def find_and_move_shared_object(self):
        extension = getattr(self, 'ext_file_ext', '.so')
        # derived module prefix
        module_prefix = f"_{self.config.library_name}"

        # Try to find compiled file with that prefix
        compiled_file = next(
            (file for file in os.listdir(self.build_temp)
             if file.startswith(module_prefix) and file.endswith(extension)),
            None
        )
        if not compiled_file:
            # fallback
            compiled_file = next(
                (file for file in os.listdir(self.build_temp)
                 if file.startswith(f"{module_prefix}.") and (
                     file.endswith(".so") or file.endswith(".pyd") or file.endswith(".dll")
                 )),
                None
            )
        if not compiled_file:
            raise RuntimeError("No extension file found in the build_temp directory.")

        src = os.path.join(self.build_temp, compiled_file)
        dst = os.path.join(self.lib_temp, compiled_file)
        os.rename(src, dst)
        return compiled_file


    def generate_init_file(self, so_file):
        init_path = os.path.join(self.lib_temp, "__init__.py")
        base_module_name = os.path.splitext(so_file)[0]
        with open(init_path, "w") as f:
            f.write(f"from .{base_module_name} import *\n")
        print(f"-- Generated __init__.py at {init_path}")

    def generate_pyi_file(self):
        module_prefix = f"_{self.config.library_name}"
        pyi_path = os.path.join(self.lib_temp, f"{module_prefix}.pyi")
        generate_stub(module_prefix, pyi_path, self.lib_temp)
        print(f"-- Generated .pyi file: {pyi_path}")

    def copy_generated_files(self, build_temp, destinations):
        # The subdir name again from config
        source_dir = os.path.join(build_temp, self.config.library_name)

        if not os.path.exists(source_dir):
            cli_print(f"Source directory {source_dir} does not exist.", level="error")
            raise RuntimeError(f"Source directory {source_dir} does not exist.")

        # Iterate over all destination directories and copy the contents
        for dest_dir in destinations:
            # Create the destination directory if it does not exist
            dest_path = os.path.join(dest_dir, self.config.library_name)
            os.makedirs(dest_path, exist_ok=True)

            for file in os.listdir(source_dir):
                src_file = os.path.join(source_dir, file)
                dst_file = os.path.join(dest_path, file)
                copyfile(src_file, dst_file)
            
            print(f"-- Copied generated files to {dest_path}")


class CustomBuildPy(build_py):
    def __init__(self, dist, config=None):
        self.config: BuildConfig = config
        super().__init__(dist)

    def run(self):
        super().run()


class CustomTestRunner(Command):
    def __init__(self, dist, config=None):
        self.config: BuildConfig = config
        super().__init__(dist)

    description = "Run Python tests"
    user_options = [
        ('test=', 't', 'Specific test to run (test name without extension)'),
        ('list', None, 'List available tests'),
    ]

    def initialize_options(self):
        self.test = None
        self.list = False

    def finalize_options(self):
        pass

    def run(self):
        # Maybe also define a tests directory in config
        tests_dir = os.path.abspath(self.config.tests_directory)

        if self.list:
            self.list_tests(tests_dir)
        else:
            self.run_python_tests(tests_dir)

    def list_tests(self, python_test_dir):
        print("Available Python tests:")
        python_tests = self.find_python_tests(python_test_dir)
        for test in python_tests:
            print(f"  {test}")

    def find_python_tests(self, python_test_dir):
        if not os.path.exists(python_test_dir):
            print(f"No Python test directory found at {python_test_dir}")
            return []
        return [
            file[:-3] for file in os.listdir(python_test_dir)
            if file.endswith(".py") and not file.startswith("__")
        ]

    def run_python_tests(self, python_test_dir):
        pytest_args = ["pytest", "-s"]
        python_tests = self.find_python_tests(python_test_dir)

        if self.test:
            if self.test in python_tests:
                pytest_args.append(os.path.join(python_test_dir, f"{self.test}.py"))
            else:
                print(f"Python test {self.test} not found.")
                return
        else:
            pytest_args.append(python_test_dir)

        subprocess.run(pytest_args)
