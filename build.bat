@echo off
setlocal

set BUILD_DIR=.build
set EXECUTABLE=Sandbox\Sandbox.exe
set EXAMPLES=Examples\example.py
set TESTING=Examples\tester.py

REM Ensure the build directory exists
if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

REM Function to build the project
:build
echo Building the project...
cd %BUILD_DIR%
cmake ..
cmake --build . --config Release
cd ..
goto :eof

REM Function to run the executable
:run
call :build
echo Running the executable...
%BUILD_DIR%\%EXECUTABLE%
goto :eof

REM Function to run the Python examples
:examples
echo Running Python examples...
python %EXAMPLES%
goto :eof

REM Function to run the tests
:tests
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

REM Function to clean the build directory
:clean
if exist %BUILD_DIR% (
    echo Cleaning up...
    rmdir /S /Q %BUILD_DIR%
)
goto :eof

REM Argument handling
if "%1" == "build" goto build
if "%1" == "run" goto run
if "%1" == "examples" goto examples
if "%1" == "tests" goto tests
if "%1" == "clean" goto clean

echo Usage: build.bat {build|run|examples|tests|clean}
