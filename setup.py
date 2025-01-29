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
        'publish': CustomPublish
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
