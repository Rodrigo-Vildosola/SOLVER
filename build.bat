@echo off
setlocal

:: Directories and file paths
set BUILD_DIR=.build
set EXECUTABLE=Sandbox\Release\Sandbox.exe
set EXAMPLES=Examples\example.py
set TESTING=Examples\tester.py
set PYTHON_OUTPUT_DIR=Examples\solver
set SOLVER_PYTHON_LIB=_SOLVER_PYTHON.pyd
set SOLVER_PYTHON_SCRIPT=solver.py

:: Ensure the build directory exists
if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

:: Function to build the project
:build
echo Building the project using Visual Studio generator...
cd %BUILD_DIR%
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
cd ..
goto :eof

:: Function to copy Python libraries
:copy_python_lib
echo Copying Python libraries...
if not exist %PYTHON_OUTPUT_DIR% (
    mkdir %PYTHON_OUTPUT_DIR%
)

:: Determine the shared library extension (on Windows, it's pyd)

:: Copy the shared library and Python script
copy %BUILD_DIR%\Solver\Release\%SOLVER_PYTHON_LIB% %PYTHON_OUTPUT_DIR%\%SOLVER_PYTHON_LIB%
copy %BUILD_DIR%\Solver\%SOLVER_PYTHON_SCRIPT% %PYTHON_OUTPUT_DIR%\%SOLVER_PYTHON_SCRIPT%

echo Python libraries copied to %PYTHON_OUTPUT_DIR%.
goto :eof

:: Function to run the executable
:run
call :build
call :copy_python_lib
echo Running the executable...
%BUILD_DIR%\%EXECUTABLE%
goto :eof

:: Function to run the Python examples
:examples
call :build
call :copy_python_lib
echo Running Python examples...
python %EXAMPLES%
goto :eof

:: Function to run the tests
:tests
call :build
call :copy_python_lib
echo Running tests...

echo Checking Python version...
python --version

echo Verifying installed Python packages...
pip list

echo Activating virtual environment...
call .venv\Scripts\activate

echo Executing Python tests from %TESTING%
python %TESTING%

echo Tests finished.
goto :eof

:: Function to clean the build directory and remove copied Python libs
:clean
if exist %BUILD_DIR% (
    echo Cleaning up build directory...
    rmdir /S /Q %BUILD_DIR%
)

if exist %PYTHON_OUTPUT_DIR%\%SOLVER_PYTHON_LIB%.pyd (
    echo Removing Python shared library and script...
    del /Q %PYTHON_OUTPUT_DIR%\%SOLVER_PYTHON_LIB%.pyd
    del /Q %PYTHON_OUTPUT_DIR%\%SOLVER_PYTHON_SCRIPT%
)

echo Clean completed.
goto :eof

:: Argument handling
if "%1" == "build" goto build
if "%1" == "run" goto run
if "%1" == "examples" goto examples
if "%1" == "tests" goto tests
if "%1" == "clean" goto clean

echo Usage: build.bat {build|run|examples|tests|clean}
