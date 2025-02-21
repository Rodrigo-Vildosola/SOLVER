# setup.py
import os
from setuptools import setup, find_packages, Extension
from setuptools.command.build_ext import build_ext


this_dir = os.path.abspath(os.path.dirname(__file__))

# Read the version from solver/VERSION
with open(os.path.join(this_dir, "solver", "VERSION"), encoding="utf-8") as f:
    version = f.read().strip()

# This "dummy" extension does not compile anything itself.
# We only define it so that setuptools knows we have an extension module.
solver_ext = Extension(
    "solver._solver",  # The extension name must match the actual module path
    sources=[],        # No sources here, because you build externally
)


class NoopBuildExt(build_ext):
    """
    If an Extension has no sources, skip compilation. 
    This lets us include a pre-compiled .so/.pyd in the package
    while still getting a platform-specific wheel.
    """
    def build_extension(self, ext):
        if not ext.sources:
            # No sources => Do nothing, just trust that the .so was built externally
            print(f"Skipping build for extension '{ext.name}' (no sources).")
        else:
            super().build_extension(ext)

setup(
    name="solver",
    version=version,
    description="Python and C++ math expression solver and parsing library",
    author="Rodrigo Vildosola",
    url="https://github.com/Rodrigo-Vildosola/SOLVER",

    # Your package directory is "solver/"
    packages=find_packages(),
    package_data={
        # Include the built shared library and other files
        "solver": [
            "VERSION",
            "__init__.py",
            "_solver.pyi",
            "_solver.so",   # or .pyd / .dylib on other platforms
        ],
    },
    include_package_data=True,

    # Mark that we have an extension module
    ext_modules=[solver_ext],
    # This ensures the resulting wheel is marked as platform-specific
    # rather than a universal "py3-none-any" wheel.
    cmdclass={
        "build_ext": NoopBuildExt  # use our override
    },

    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux",
        "Operating System :: MacOS :: MacOS X",
    ],
    python_requires=">=3.8",
)
