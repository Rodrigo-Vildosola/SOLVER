from tools.commands import CustomBuild, CustomBuildExt, CustomBuildPy, CustomTestRunner
from tools.parse_config import BuildConfig
from setuptools import setup, Extension
from setuptools import Command
import subprocess
import shutil
import glob
import os

config = BuildConfig.load_from_file("build_config.json")

# Read version and readme
version = open(config.version_file, "r").read().strip()
long_description = open(config.readme_file, "r").read()

# Ensure the build directory exists
os.makedirs(config.build_directory, exist_ok=True)

class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])

class CustomClean(Command):
    user_options = []

    def initialize_options(self):
        pass
    def finalize_options(self):
        pass

    def run(self):
        build_dirs = [config.build_directory, "dist"]
        for d in build_dirs:
            if os.path.exists(d):
                shutil.rmtree(d)

        for d in config.extra_directories:
            path = os.path.join(d, config.library_name)
            if os.path.exists(path):
                shutil.rmtree(path)

        print("Cleaned directories.")

class CustomPublish(Command):
    user_options = []

    def initialize_options(self):
        pass
    def finalize_options(self):
        pass


    def run(self):
        self.run_command('clean')

        os.makedirs(config.build_directory, exist_ok=True)

        self.run_command('sdist')
        self.run_command('bdist_wheel')

        if os.path.exists(".pypirc"):
            wheels = glob.glob("dist/*.whl")
            if not wheels:
                print("No wheels found in dist/. Skipping upload.")
                return
            print("Found .pypirc. Uploading wheels...")
            cmd = ["twine", "upload"] + wheels
            print(f"Running: {' '.join(cmd)}")
            subprocess.check_call(cmd)
        else:
            print("No .pypirc found. Skipping upload.")

class CustomFormat(Command):
    user_options = []

    def initialize_options(self):
        pass
    def finalize_options(self):
        pass


    def run(self):
        # Check if clang-format is installed
        clang_format_path = shutil.which("clang-format")
        if not clang_format_path:
            raise RuntimeError("clang-format is not installed or not found in PATH.")

        print(f"Using clang-format: {clang_format_path}")

        # Collect all .cpp and .c files from source directory
        source_files = []
        if os.path.exists(config.source_directory):
            source_files = glob.glob(f"{config.source_directory}/**/*.cpp", recursive=True) + \
                           glob.glob(f"{config.source_directory}/**/*.c", recursive=True)

        # Collect all .h and .hpp files from headers directory
        header_files = []
        if os.path.exists(config.headers_directory):
            header_files = glob.glob(f"{config.headers_directory}/**/*.h", recursive=True) + \
                           glob.glob(f"{config.headers_directory}/**/*.hpp", recursive=True)

        # Merge all files to format
        all_files = source_files + header_files

        if not all_files:
            print("No source or header files found to format.")
            return

        print(f"Formatting {len(all_files)} files...")

        # Run clang-format on each file
        for file in all_files:
            print(f"Formatting: {file}")
            subprocess.run([clang_format_path, "-i", file], check=True)

        print("Code formatting complete.")

def command_factory(base_cls, config):
    class CommandSubclass(base_cls):
        def __init__(self, dist):
            super().__init__(dist, config=config)
    return CommandSubclass

setup(
    name=config.library_name,
    version=version,
    author=config.author,
    description=config.description,
    long_description=long_description,
    long_description_content_type="text/markdown",
    url=config.url,

    # Single extension with the same name as the library
    ext_modules=[CMakeExtension(config.library_name)],

    # Custom commands
    cmdclass={
        'build_ext': command_factory(CustomBuildExt, config),
        'build_py': command_factory(CustomBuildPy, config),
        'build': command_factory(CustomBuild, config),
        'test': command_factory(CustomTestRunner, config),
        'clean': CustomClean,
        'publish': CustomPublish,
        'format': CustomFormat
    },

    install_requires=config.install_requires,
    classifiers=config.classifiers,
    python_requires=config.python_requires,
    zip_safe=config.zip_safe,

    options={
        'build': {
            'build_base': config.build_directory
        },
        'egg_info': {
            'egg_base': config.build_directory
        }
    },
)
